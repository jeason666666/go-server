#ifndef MESSAGE_TRANSMIT_H
#define MESSAGE_TRANSMIT_H

#include "message_transmit.pb.h"

class MesTranServiceImpl : public MesTranService {
public:
    MesTranServiceImpl() {}
    ~MesTranServiceImpl() {}
        
    void TransmitMessage(::google::protobuf::RpcController* cntl_base,
        const MesTransReq* request,
        MesTransRes* response,
        ::google::protobuf::Closure* done);
};

class HeartBeatServiceImpl : public HeartBeatService {
public:
    HeartBeatServiceImpl() {}
    ~HeartBeatServiceImpl() {}

    void HeartBeat(::google::protobuf::RpcController* cntl_base,
        const HeartBeatReq* request,
        HeartBeatRes* response,
        ::google::protobuf::Closure* done);
};

// 改成redis
class UserOnlineServiceImpl : public UserOnlineService {
public:
    UserOnlineServiceImpl() {}
    ~UserOnlineServiceImpl() {}

    void UserOnline(::google::protobuf::RpcController* cntl_base,
        const UserOnlineReq* request,
        UserOnlineRes* response,
        ::google::protobuf::Closure* done);
};

#endif
