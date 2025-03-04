#include <gflags/gflags.h>
#include <butil/logging.h>
#include <brpc/server.h>
#include <brpc/restful.h>
#include <json2pb/pb_to_json.h>
#include <iostream>

#include "httpgateway/http_gateway.h"

int main() {
    brpc::Server server;

    HttpGatewayServiceImpl http_gateway_svc;

    if (server.AddService(&http_gateway_svc, 
                      brpc::SERVER_DOESNT_OWN_SERVICE) != 0) {
        std::cout << "Add Svc Error\n";
    }

    brpc::ServerOptions options;
    options.idle_timeout_sec = -1;
    if (server.Start(8080, &options) != 0) {
        std::cout << "start error\n";
    }

    server.RunUntilAskedToQuit();

    return 0;
}