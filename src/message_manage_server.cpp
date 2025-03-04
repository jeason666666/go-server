#include <message_manage/message_manage.h>

#include <brpc/server.h>

#include <iostream>

int main() {
    brpc::Server server;

    MessageServiceImpl message_svc;

    if (server.AddService(&message_svc,
                      brpc::SERVER_DOESNT_OWN_SERVICE) != 0) {
        std::cout << "message svc add failed\n";
    }

    brpc::ServerOptions options;
    options.idle_timeout_sec = -1;

    if (server.Start(8084, &options) != 0) {
        std::cout << "message server start failed\n";
    }

    server.RunUntilAskedToQuit();

    return 0;
}
