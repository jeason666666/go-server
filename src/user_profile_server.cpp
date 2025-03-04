#include "user_profile/user_profile.h"

#include <brpc/server.h>

#include <iostream>
#include <unordered_map>

int main() {
    brpc::Server server;

    UserProfileServiceImpl user_profile_svc;

    if (server.AddService(&user_profile_svc,
                      brpc::SERVER_DOESNT_OWN_SERVICE) != 0) {
        std::cout << "login svc add failed\n";
    }

    brpc::ServerOptions options;
    options.idle_timeout_sec = -1;

    if (server.Start(8082, &options) != 0) {
        std::cout << "user_profile server start failed\n";
    }

    server.RunUntilAskedToQuit();

    return 0;
}