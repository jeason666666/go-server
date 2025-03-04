#ifndef USER_PROFILE_H
#define USER_PROFILE_H

#include "user_profile/user_profile.pb.h"

#include <iostream>

#include <brpc/server.h>

class UserProfileServiceImpl : public UserProfileService {
public:
    UserProfileServiceImpl();
    ~UserProfileServiceImpl();

    void UserProfile(::google::protobuf::RpcController* cntl_base,
        const UserProfileRequest* request,
        UserProfileResponse* response,
        ::google::protobuf::Closure* done);
};

#endif