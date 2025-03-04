#include "message_transmit.h"

#include <brpc/server.h>
#include <spdlog/spdlog.h>

#include <iostream>
#include <queue>
#include <string>
#include <map>

// 改成消息队列
std::map<int, std::queue<std::string>> messages;
std::set<int> onlines;

void PushQueue(int friend_id, std::string message) {
    messages[friend_id].push(message);
}

std::pair<bool, std::string> PopQueue(int id) {
    if (messages[id].empty()) {
        return {false, ""};
    }
    else {
        std::string mes = messages[id].front();
        messages[id].pop();
        return {true, mes};
    }
}

void MesTranServiceImpl::TransmitMessage(::google::protobuf::RpcController* cntl_base,
    const MesTransReq* request,
    MesTransRes* response,
    ::google::protobuf::Closure* done) {

    brpc::ClosureGuard done_guard(done);
    
    // brpc::Controller* cntl = static_cast<brpc::Controller*>(cntl_base);

    std::cout << request->friend_id() << ' ' << request->time_stamp() << ' ' << request->message() << '\n';
    // 发送内容的长度有上限，需要改进
    char context[1024];
    sprintf(context, "%d %ld %s", request->friend_id(), request->time_stamp(), request->message().c_str());
    std::cout << context << '\n';
    
    PushQueue(request->friend_id(), context);
}


// TODO
// 心跳包检测用户掉线
void HeartBeatServiceImpl::HeartBeat(::google::protobuf::RpcController* cntl_base,
    const HeartBeatReq* request,
    HeartBeatRes* response,
    ::google::protobuf::Closure* done) {
    
    brpc::ClosureGuard done_guard(done);
    
    onlines.insert(request->id());
    std::pair<bool, std::string> mes = PopQueue(request->id());
    if (mes.first) {
        response->set_flag(true);
        response->set_message(mes.second);
    }
    else {
        response->set_flag(false);
    }
}

void UserOnlineServiceImpl::UserOnline(::google::protobuf::RpcController* cntl_base,
    const UserOnlineReq* request,
    UserOnlineRes* response,
    ::google::protobuf::Closure* done) {

    brpc::ClosureGuard done_guard(done);
    
    if (onlines.count(request->id())) {
        response->set_flag(true);
    }
    else {
        response->set_flag(false);
    }
}
