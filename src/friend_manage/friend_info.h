#ifndef FRIEND_INFO_H
#define FRIEND_INFO_H

#include "friend_info.pb.h"


class FriendRequestServiceImpl : public FriendRequestService {
public:
    FriendRequestServiceImpl();
    ~FriendRequestServiceImpl();
public:
    void FriendRequest(::google::protobuf::RpcController* cntl_base,
        const FriendRequestReq* request,
        FriendRequestRes* response,
        ::google::protobuf::Closure* done);
};


class FriendGetServiceImpl : public FriendGetService {
public:
    FriendGetServiceImpl() {}
    ~FriendGetServiceImpl() {}
public:
    void FriendGet(::google::protobuf::RpcController* cntl_base,
        const FriendGetReq* request,
        FriendGetRes* response,
        ::google::protobuf::Closure* done);
};

#endif // FRIEND_INFO_H