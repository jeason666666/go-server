#include "friend_manage/friend_info.h"
#include "httpgateway/http_gateway.h"
#include "login/login.h"
#include "user_profile/user_profile.h"
#include "message_manage/message_manage.h"
#include "message_transmit/message_transmit.h"

#include <brpc/channel.h>
#include <brpc/server.h>
#include <jsoncons/json.hpp>
#include "spdlog/spdlog.h"

#include <iostream>

/***********************************************************************
 * 
 * login            8081
 * register
 * user_profile     8082
 * add_friend       8083
 *           get
 *           add
 * get_friend
 * message          8084
 * (message transmit 8085  (8090))
 **********************************************************************/


// TODOLIST
// 优化数据库结构
// 同意添加后删除记录
// json转protobuf应该由每个业务自己做

void HttpGatewayServiceImpl::Echo(::google::protobuf::RpcController* cntl_base,
            const HttpRequest* request,
            HttpResponse* response,
            ::google::protobuf::Closure* done) {

    std::cout << "ECHO!!!!\n";
    brpc::ClosureGuard done_guard(done);
    brpc::Controller* cntl = static_cast<brpc::Controller*>(cntl_base);

    std::cout << cntl->http_request().unresolved_path() << '\n';

    std::cout << cntl->request_attachment() << '\n';

    // LOGIN service
    // to 8081
    if (cntl->http_request().unresolved_path().substr(0, 5) == "login") {
        jsoncons::json j = jsoncons::json::parse(cntl->request_attachment().to_string());

        brpc::Channel channel;
        brpc::ChannelOptions options;
        options.protocol = "h2:grpc";
        channel.Init("127.0.0.1:8081", &options);

        LoginService_Stub stub(&channel);

        LoginRequest request;
        LoginResponse response;
        brpc::Controller channel_cntl;

        request.set_op_type(static_cast<OPERTYPE>(j["op_type"].as<int>()));
        request.set_id(j["id"].as<int>());
        request.set_password(j["password"].as<std::string>());
        stub.Login(&channel_cntl, &request, &response, NULL);

        jsoncons::json res;
        res["status"] = static_cast<int>(response.status());

        cntl->http_response().set_content_type("application/json");
        std::string str;
        jsoncons::encode_json(res, str);
        cntl->response_attachment().append(str);
        std::cout << str << '\n';
    }
    else if (cntl->http_request().unresolved_path().substr(0, 8) == "register") {
        jsoncons::json j = jsoncons::json::parse(cntl->request_attachment().to_string());
        
        brpc::Channel channel;
        brpc::ChannelOptions options;
        options.protocol = "h2:grpc";
        channel.Init("127.0.0.1:8081", &options);

        LoginService_Stub stub(&channel);

        LoginRequest request;
        LoginResponse response;
        brpc::Controller channel_cntl;

        request.set_op_type(static_cast<OPERTYPE>(j["op_type"].as<int>()));
        request.set_username(j["username"].as<std::string>());
        request.set_password(j["password"].as<std::string>());
        stub.Login(&channel_cntl, &request, &response, NULL);

        jsoncons::json res;
        res["status"] = static_cast<int>(response.status());

        cntl->http_response().set_content_type("application/json");
        std::string str;
        jsoncons::encode_json(res, str);
        cntl->response_attachment().append(str);
        std::cout << str << '\n';
    }
    // user profile service
    // to 8082
    else if (cntl->http_request().unresolved_path().substr(0, 12) == "user_profile") {
        jsoncons::json j = jsoncons::json::parse(cntl->request_attachment().to_string());
        
        brpc::Channel channel;
        brpc::ChannelOptions options;
        options.protocol = "h2:grpc";
        channel.Init("127.0.0.1:8082", &options);

        UserProfileService_Stub stub(&channel);

        UserProfileRequest request;
        UserProfileResponse response;
        brpc::Controller channel_cntl;

        request.set_id(j["id"].as<int>());
        stub.UserProfile(&channel_cntl, &request, &response, NULL);

        jsoncons::json res;
        res["username"] = response.username();

        cntl->http_response().set_content_type("application/json");
        std::string str;
        jsoncons::encode_json(res, str);
        cntl->response_attachment().append(str);
        std::cout << str << '\n';
    }
    // add friend service
    // to 8083
    else if (cntl->http_request().unresolved_path().substr(0, 10) == "add_friend") {
        jsoncons::json j = jsoncons::json::parse(cntl->request_attachment().to_string());
        
        brpc::Channel channel;
        brpc::ChannelOptions options;
        options.protocol = "h2:grpc";
        channel.Init("127.0.0.1:8083", &options);

        FriendRequestService_Stub stub(&channel);

        FriendRequestReq request;
        FriendRequestRes response;
        brpc::Controller channel_cntl;
        request.set_op_type(static_cast<FRIEND_REQUEST_OP>(j["op_type"].as<int>()));
        request.set_id(j["id"].as<int>());
        if (request.op_type() == FRIEND_REQUEST_OP::ADD_FRIEND 
         || request.op_type() == FRIEND_REQUEST_OP::AGREE_ADD) {
            request.set_friend_id(j["friend_id"].as<int>());
        }
        stub.FriendRequest(&channel_cntl, &request, &response, NULL);

        if (request.op_type() == FRIEND_REQUEST_OP::GET_FRIEND) {
            jsoncons::json res;
            res["ids"] = jsoncons::json::array(response.ids().begin(), response.ids().end());
            res["usernames"] = jsoncons::json::array(response.usernames().begin(), response.usernames().end());

            cntl->http_response().set_content_type("application/json");
            std::string str;
            jsoncons::encode_json(res, str);
            cntl->response_attachment().append(str);
            std::cout << str << '\n';
        }
    }
    // get friend service
    // to 8083
    else if (cntl->http_request().unresolved_path().substr(0, 10) == "get_friend") {
        jsoncons::json j = jsoncons::json::parse(cntl->request_attachment().to_string());
        
        brpc::Channel channel;
        brpc::ChannelOptions options;
        options.protocol = "h2:grpc";
        channel.Init("127.0.0.1:8083", &options);

        FriendGetService_Stub stub(&channel);

        FriendGetReq request;
        FriendGetRes response;
        brpc::Controller channel_cntl;

        request.set_id(j["id"].as<int>());
        stub.FriendGet(&channel_cntl, &request, &response, NULL);

        jsoncons::json res;
        res["ids"] = jsoncons::json::array(response.friend_ids().begin(), response.friend_ids().end());
        res["usernames"] = jsoncons::json::array(response.usernames().begin(), response.usernames().end());

        cntl->http_response().set_content_type("application/json");
        std::string str;
        jsoncons::encode_json(res, str);
        cntl->response_attachment().append(str);
        std::cout << str << '\n';
    }
    // message_manage
    // to 8084
    else if (cntl->http_request().unresolved_path().substr(0, 7) == "message") {
        jsoncons::json j = jsoncons::json::parse(cntl->request_attachment().to_string());

        brpc::Channel channel;
        brpc::ChannelOptions options;
        options.protocol = "h2:grpc";
        channel.Init("127.0.0.1:8084", &options);

        MessageService_Stub stub(&channel);
        
        MessageRequest request;
        MessageResponse response;
        brpc::Controller channel_cntl;

        request.set_op_type(static_cast<MESSAGE_OP>(j["op_type"].as<int>()));
        request.set_id(j["id"].as<int>());
        request.set_friend_id(j["friend_id"].as<int>());
        request.set_time_stamp(j["time_stamp"].as<int64_t>());
        if (request.op_type() == MESSAGE_OP::SEND) {
            request.set_send_id(j["send_id"].as<int>());
            request.set_message(j["message"].as<std::string>());
        }
        
        stub.GetMessage(&channel_cntl, &request, &response, NULL);

        if (request.op_type() == MESSAGE_OP::GET) {
            jsoncons::json res;
            res["contents"] = jsoncons::json::array();
            res["timestamps"] = jsoncons::json::array();
            res["send_ids"] = jsoncons::json::array();
            for (auto& message : response.messages()) {
                res["contents"].push_back(message.content());
                res["timestamps"].push_back(message.time_stamp());
                res["send_ids"].push_back(message.send_id());
            }
            
            cntl->http_response().set_content_type("application/json");
            std::string str;
            jsoncons::encode_json(res, str);
            cntl->response_attachment().append(str);
            std::cout << str << '\n';
        }
    }
    // 心跳包
    // to 8085
    else if (cntl->http_request().unresolved_path().substr(0, 10) == "heart_beat") {
        jsoncons::json j = jsoncons::json::parse(cntl->request_attachment().to_string());
        brpc::Channel channel;
        brpc::ChannelOptions options;
        options.protocol = "h2:grpc";
        channel.Init("127.0.0.1:8085", &options);

        HeartBeatReq request;
        HeartBeatRes response;
        brpc::Controller channel_cntl;

        HeartBeatService_Stub stub(&channel);
        request.set_id(j["id"].as<int>());
        stub.HeartBeat(&channel_cntl, &request, &response, NULL);

        jsoncons::json res;
        res["flag"] = response.flag();
        res["message"] = response.message();
        cntl->http_response().set_content_type("application/json");
        std::string str;
        jsoncons::encode_json(res, str);
        cntl->response_attachment().append(str);
        std::cout << str << '\n';
    }
    else {
        std::cout << "unreoslved_path :" << cntl->http_request().unresolved_path() << '\n';
    }
}