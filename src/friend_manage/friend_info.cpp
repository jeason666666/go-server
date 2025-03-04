#include "friend_info.h"
#include "user_profile/user_profile.h"

#include <brpc/channel.h>
#include <brpc/server.h>
#include <jsoncons/json.hpp>
#include <mysql/mysql.h>

#include <iostream>
#include <vector>

static MYSQL* mysql = NULL;

// TODO
// 支持长时间连接
static void InitMysql() {
    mysql = mysql_init(NULL);

    if (mysql_real_connect(mysql,
                       "127.0.0.1",
                       "ljs",
                       "123456",
                       "go_server",
                       3306,
                       NULL,
                       0) == NULL) {
        std::cout << "连接失败:" << mysql_error(mysql) << "\n";
    }
}

static void RequestAddFriend(int id, int friend_id) {
    char add_sql[100];
    sprintf(add_sql, "INSERT INTO friend_request (id, friend_id) VALUES (%d, %d)", id, friend_id);
    std::cout << add_sql << '\n';
    if (mysql_query(mysql, add_sql) == 0) {

    }
    else {
        std::cout << "add friend request failed\n";
    }
}

static void AddFriend(int id, int friend_id) {
    char add_sql[100];
    sprintf(add_sql, "DELETE FROM friend_request where friend_id = %d AND id = %d", friend_id, id);
    sprintf(add_sql, "INSERT INTO friend (id, friend_id) VALUES (%d, %d)", id, friend_id);
    std::cout << add_sql << '\n';
    if (mysql_query(mysql, add_sql) == 0) {
        sprintf(add_sql, "INSERT INTO friend (id, friend_id) VALUES (%d, %d)", friend_id, id);
        if (mysql_query(mysql, add_sql) == 0) {

        }
        else {
            std::cout << "add friend failed\n";
            std::cout << mysql_error(mysql) << '\n';
        }
    }
    else {
        std::cout << "add friend failed\n";
        std::cout << mysql_error(mysql) << '\n';
    }
}

static std::vector<int> FriendRequests(int id) {
    char query_sql[100];
    sprintf(query_sql, "SELECT id from friend_request where friend_id = %d", id);
    std::cout << query_sql << '\n';

    mysql_query(mysql, query_sql);
    MYSQL_RES* res = mysql_store_result(mysql);

    int num_fields = mysql_num_fields(res);
    int num_rows = mysql_num_rows(res);
    
    std::vector<int> ret;
    MYSQL_ROW row;
    while (row = mysql_fetch_row(res)) {
        ret.push_back(atoi(row[0]));
    }
    return ret;
}

FriendRequestServiceImpl::FriendRequestServiceImpl() {
    InitMysql();
}

FriendRequestServiceImpl::~FriendRequestServiceImpl() {
    mysql_close(mysql);
    std::cout << "~mysql" << '\n';
}

void FriendRequestServiceImpl::FriendRequest(::google::protobuf::RpcController* cntl_base,
    const FriendRequestReq* request,
    FriendRequestRes* response,
    ::google::protobuf::Closure* done) {
    
    brpc::ClosureGuard done_guard(done);

    brpc::Controller* cntl = static_cast<brpc::Controller*>(cntl_base);

    if (request->op_type() == FRIEND_REQUEST_OP::ADD_FRIEND) {
        RequestAddFriend(request->id(), request->friend_id());
    }
    else if (request->op_type() == FRIEND_REQUEST_OP::GET_FRIEND) {
        brpc::Channel channel;
        brpc::ChannelOptions options;
        options.protocol = "h2:grpc";
        channel.Init("127.0.0.1:8082", &options);

        UserProfileService_Stub stub(&channel);

        UserProfileRequest user_profile_request;
        UserProfileResponse user_profile_response;
        brpc::Controller channel_cntl;

        std::vector<int> ids = FriendRequests(request->id());
        for (int i = 0; i < ids.size(); i ++) {
            std::cout << "id" << i << ":" << ids[i] << '\n';
            user_profile_request.set_id(ids[i]);
            stub.UserProfile(&channel_cntl, &user_profile_request, &user_profile_response, NULL);

            response->add_ids(ids[i]);
            response->add_usernames(user_profile_response.username());
        }
    }
    else if (request->op_type() == FRIEND_REQUEST_OP::AGREE_ADD) {
        AddFriend(request->id(), request->friend_id());
    }
    else {
        std::cout << "NO SUCH OP\n";
    }
}

static std::vector<int> get_friend_ids(int id) {
    char query_sql[100];
    sprintf(query_sql, "SELECT friend_id from friend where id = %d", id);
    std::cout << query_sql << '\n';

    if (mysql_query(mysql, query_sql) != 0) {

    }
    else {
        std::cout << "query failed\n";
    }
    MYSQL_RES* res = mysql_store_result(mysql);

    int num_fields = mysql_num_fields(res);
    int num_rows = mysql_num_rows(res);
    
    std::vector<int> ret;
    MYSQL_ROW row;
    while (row = mysql_fetch_row(res)) {
        ret.push_back(atoi(row[0]));
    }
    return ret;
}

void FriendGetServiceImpl::FriendGet(::google::protobuf::RpcController* cntl_base,
    const FriendGetReq* request,
    FriendGetRes* response,
    ::google::protobuf::Closure* done) {

    brpc::ClosureGuard done_guard(done);

    brpc::Controller* cntl = static_cast<brpc::Controller*>(cntl_base);


    brpc::Channel channel;
    brpc::ChannelOptions options;
    options.protocol = "h2:grpc";
    channel.Init("127.0.0.1:8082", &options);

    UserProfileService_Stub stub(&channel);

    UserProfileRequest user_profile_request;
    UserProfileResponse user_profile_response;
    brpc::Controller channel_cntl;

    std::vector<int> ids = get_friend_ids(request->id());

    for (auto& id : ids) {
        user_profile_request.set_id(id);
        stub.UserProfile(&channel_cntl, &user_profile_request, &user_profile_response, NULL);

        response->add_friend_ids(id);
        response->add_usernames(user_profile_response.username());
    }
}
