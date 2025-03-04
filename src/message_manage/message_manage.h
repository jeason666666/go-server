#ifndef MESSAGE_MANAGE_H
#define MESSAGE_MANAGE_H

#include "message.pb.h"

class MessageServiceImpl : public MessageService
{
public:
    MessageServiceImpl();
    ~MessageServiceImpl();
    
    void GetMessage(::google::protobuf::RpcController* cntl_base,
        const MessageRequest* request,
        MessageResponse* response,
        ::google::protobuf::Closure* done);
};

#endif