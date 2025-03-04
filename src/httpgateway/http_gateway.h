#ifndef HTTP_GATEWAY_H
#define HTTP_GATEWAY_H

#include "http_gateway.pb.h"

class HttpGatewayServiceImpl : public HttpService {
public:
    HttpGatewayServiceImpl() {}
    virtual ~HttpGatewayServiceImpl() {}

    void Echo(::google::protobuf::RpcController* cntl_base,
              const HttpRequest* request,
              HttpResponse* response,
              ::google::protobuf::Closure* done);
};

#endif // HTTP_GATEWAY_H
