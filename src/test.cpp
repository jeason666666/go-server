#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <spdlog/spdlog.h>
#include <brpc/server.h>

#include <map>
#include <unordered_map>

typedef websocketpp::server<websocketpp::config::asio> server;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

typedef server::message_ptr message_ptr;

server message_transmit_server;

// 自定义比较函数对象
struct ConnectionHdlCompare {
    bool operator()(const websocketpp::connection_hdl& a, const websocketpp::connection_hdl& b) const {
        // 通过比较底层指针来实现排序
        auto raw_ptr_a = a.lock().get();
        auto raw_ptr_b = b.lock().get();
        return raw_ptr_a < raw_ptr_b;
    }
};

std::unordered_map<int, websocketpp::connection_hdl> handles;
std::map<websocketpp::connection_hdl, int, ConnectionHdlCompare> ids;

void on_message(server* s, websocketpp::connection_hdl hdl, message_ptr msg) {
    spdlog::info("{} online", atoi(msg->get_payload().c_str()));
    handles[atoi(msg->get_payload().c_str())] = hdl;
    ids[hdl] = atoi(msg->get_payload().c_str());

    // std::cout << "on_message called with hdl: " << hdl.lock().get()
    //           << " and message: " << msg->get_payload()
    //           << std::endl;

    // check for a special command to instruct the server to stop listening so
    // it can be cleanly exited.
    // if (msg->get_payload() == "stop-listening") {
    //     s->stop_listening();
    //     return;
    // }

    // try { 
    //     s->send(hdl, msg->get_payload(), msg->get_opcode());
    // } catch (websocketpp::exception const & e) {
    //     std::cout << "Echo failed because: "
    //               << "(" << e.what() << ")" << std::endl;
    // }
}

void on_close(server* s, websocketpp::connection_hdl hdl) {
    int id = ids[hdl];
    spdlog::info("{} offline", id);

    handles.erase(id);
    ids.erase(hdl);
}

void Init() {
    try {
        message_transmit_server.set_access_channels(websocketpp::log::alevel::all);
        message_transmit_server.clear_access_channels(websocketpp::log::alevel::frame_payload);

        message_transmit_server.init_asio();

        message_transmit_server.set_message_handler(bind(&on_message, &message_transmit_server, ::_1, ::_2));
        message_transmit_server.set_close_handler(bind(&on_close, &message_transmit_server, ::_1));

        message_transmit_server.listen(8090);

        message_transmit_server.start_accept();

        message_transmit_server.run();
    } catch (websocketpp::exception const& e) {
        spdlog::error(e.what());
    } catch (...) {
        spdlog::error("other exception\n");
    }
}

int main() {
    Init();
    brpc::Server server;

    brpc::ServerOptions options;
    options.idle_timeout_sec = -1;

    if (server.Start(8084, &options) != 0) {
        std::cout << "message server start failed\n";
    }

    server.RunUntilAskedToQuit();

    return 0;
}
