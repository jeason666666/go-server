#include "message_manage.h"
#include "message_transmit/message_transmit.h"

#include <brpc/channel.h>
#include <brpc/server.h>
#include <mysql/mysql.h>

#include <iostream>
#include <string>
#include <vector>

static MYSQL* mysql = NULL;

// todo
// 支持长时间连接
static void initMysql() {
    mysql = mysql_init(NULL);

    if (mysql_real_connect(mysql,
                       "127.0.0.1",
                       "ljs",
                       "123456",
                       "go_server",
                       3306,
                       NULL,
                       0) == NULL) {
        std::cout << "连接失败:" << mysql_error(mysql) << "\n";
    }
}

static void SendMessage(int id, int friend_id, int send_id, std::string message, int64_t time_stamp) {
    std::cout << "SEND\n";
    UserOnlineReq urequest;
    UserOnlineRes uresponse;
    {
        brpc::Channel channel;
        brpc::ChannelOptions options;
        options.protocol = "h2:grpc";
        channel.Init("127.0.0.1:8085", &options);

        UserOnlineService_Stub stub(&channel);
        
        brpc::Controller channel_cntl;

        urequest.set_id(friend_id);
        std::cout << urequest.id() << '\n';
        stub.UserOnline(&channel_cntl, &urequest, &uresponse, NULL);
        std::cout << "uresponse : " << uresponse.flag() << '\n';
    }
    if (uresponse.flag()) {
        brpc::Channel channel;
        brpc::ChannelOptions options;
        options.protocol = "h2:grpc";
        channel.Init("127.0.0.1:8085", &options);

        MesTranService_Stub stub(&channel);
        
        MesTransReq request;
        MesTransRes response;
        brpc::Controller channel_cntl;
        request.set_id(id);
        request.set_friend_id(friend_id);
        request.set_message(message);
        request.set_time_stamp(time_stamp);

        stub.TransmitMessage(&channel_cntl, &request, &response, NULL);
    }

    char add_sql[1024];
    sprintf(add_sql, "INSERT INTO messages (id, friend_id, send_id, message, timestamp) VALUES (%d, %d, %d, \'%s\', %ld)"
          , id, friend_id, send_id, message.c_str(), time_stamp);

    if (mysql_query(mysql, add_sql) == 0) {
        std::cout << add_sql << '\n';
    }
    else {
        std::cout << "Stored Message Failed\n";
    }
}

static std::vector<std::pair<std::string, int64_t>> GetMessages(const MessageRequest* request, MessageResponse* response) {
    std::cout << "GET\n";
    char query_sql[1024];
    sprintf(query_sql, "SELECT send_id, message, timestamp from messages where id = %d AND friend_id = %d AND timestamp > %ld"
          , request->id(), request->friend_id(), request->time_stamp());

    std::vector<std::pair<std::string, int64_t>> ret;
    if (mysql_query(mysql, query_sql) == 0) {
        MYSQL_RES *result = mysql_store_result(mysql);

        int num_fields = mysql_num_fields(result);
        MYSQL_ROW row;

        while (row = mysql_fetch_row(result)) {
            one_message* cur = response->add_messages();
            cur->set_send_id(atoi(row[0]));
            cur->set_content(row[1]);
            cur->set_time_stamp(std::stoll(row[2]));
        }
        for (auto& message : ret) {
            std::cout << message.first << "  :  " << message.second << '\n';
        }

        mysql_free_result(result);

        std::cout << query_sql << '\n';
    }
    else {
        std::cout << "Get Messages Failed\n";
    }

    sprintf(query_sql, "SELECT send_id, message, timestamp from messages where id = %d AND friend_id = %d AND timestamp > %ld"
        , request->friend_id(), request->id(), request->time_stamp());

    if (mysql_query(mysql, query_sql) == 0) {
        MYSQL_RES *result = mysql_store_result(mysql);

        int num_fields = mysql_num_fields(result);
        MYSQL_ROW row;

        while (row = mysql_fetch_row(result)) {
            one_message* cur = response->add_messages();
            cur->set_send_id(atoi(row[0]));
            cur->set_content(row[1]);
            cur->set_time_stamp(std::stoll(row[2]));
        }
        for (auto& message : ret) {
            std::cout << message.first << "  :  " << message.second << '\n';
        }

        mysql_free_result(result);

        std::cout << query_sql << '\n';
    }
    else {
        std::cout << "Get Messages Failed\n";
    }
    return ret;
}

MessageServiceImpl::MessageServiceImpl()
{
    initMysql();
}

MessageServiceImpl::~MessageServiceImpl()
{
    mysql_close(mysql);
}

void MessageServiceImpl::GetMessage(::google::protobuf::RpcController* cntl_base,
    const MessageRequest* request,
    MessageResponse* response,
    ::google::protobuf::Closure* done)
{
    brpc::ClosureGuard done_guard(done);
    
    brpc::Controller* cntl = static_cast<brpc::Controller*>(cntl_base);

    if (request->op_type() == MESSAGE_OP::SEND) {
        SendMessage(request->id(), request->friend_id(), request->send_id(), request->message(), request->time_stamp());
    }
    else if (request->op_type() == MESSAGE_OP::GET) {
        std::vector<std::pair<std::string, int64_t>> messages = GetMessages(request, response);
        for (auto& message : messages) {
            one_message* cur = response->add_messages();
            cur->set_content(message.first);
            cur->set_time_stamp(message.second);
        }
    }
}

