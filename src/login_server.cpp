#include "login/login.h"

#include <brpc/server.h>

#include <iostream>

int main() {
    brpc::Server server;

    LoginServiceImpl login_svc;

    if (server.AddService(&login_svc,
                      brpc::SERVER_DOESNT_OWN_SERVICE) != 0) {
        std::cout << "login svc add failed\n";
    }

    brpc::ServerOptions options;
    options.idle_timeout_sec = -1;

    if (server.Start(8081, &options) != 0) {
        std::cout << "login server start failed\n";
    }

    server.RunUntilAskedToQuit();

    return 0;
}