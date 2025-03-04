#ifndef LOGIN_H
#define LOGIN_H

#include "login/login_info.pb.h"

class LoginServiceImpl : public LoginService {
public:
    LoginServiceImpl();
    virtual ~LoginServiceImpl();

    void Login(::google::protobuf::RpcController* cntl_base,
               const LoginRequest* request,
               LoginResponse* response,
               ::google::protobuf::Closure* done);
};

#endif // LOGIN_H