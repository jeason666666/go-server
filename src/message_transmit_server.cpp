#include "message_transmit/message_transmit.h"

#include <brpc/server.h>

#include <iostream>

int main() {
    brpc::Server server;

    MesTranServiceImpl message_transmit_svc;

    if (server.AddService(&message_transmit_svc,
                      brpc::SERVER_DOESNT_OWN_SERVICE) != 0) {
        std::cout << "message transmit svc add failed\n";
    }

    HeartBeatServiceImpl heart_beat_svc;

    if (server.AddService(&heart_beat_svc,
                      brpc::SERVER_DOESNT_OWN_SERVICE) != 0) {
        std::cout << "heart beat svc failed\n";
    }

    UserOnlineServiceImpl user_online_svc;

    if (server.AddService(&user_online_svc,
                      brpc::SERVER_DOESNT_OWN_SERVICE) != 0) {
        std::cout << "user online svc failed\n";
    }

    brpc::ServerOptions options;
    options.idle_timeout_sec = -1;

    if (server.Start(8085, &options) != 0) {
        std::cout << "message transmit server start failed\n";
    }

    server.RunUntilAskedToQuit();

    return 0;
}
