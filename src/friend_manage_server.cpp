#include "friend_manage/friend_info.h"

#include <brpc/server.h>
#include "spdlog/spdlog.h"

#include <iostream>

int main() {
    brpc::Server server;

    FriendRequestServiceImpl friend_request_svc;
    FriendGetServiceImpl friend_get_svc;

    if (server.AddService(&friend_request_svc,
                      brpc::SERVER_DOESNT_OWN_SERVICE) != 0) {
        std::cout << "friend request svc add failed\n";
    }

    if (server.AddService(&friend_get_svc,
                      brpc::SERVER_DOESNT_OWN_SERVICE) != 0) {
        std::cout << "friend get svc add failed\n";
    }
    

    brpc::ServerOptions options;
    options.idle_timeout_sec = -1;

    if (server.Start(8083, &options) != 0) {
        std::cout << "friend request server start failed\n";
    }

    server.RunUntilAskedToQuit();

    return 0;
}