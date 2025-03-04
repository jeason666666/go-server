#include "user_profile/user_profile.h"

#include <unordered_map>

#include <hiredis/hiredis.h>
#include <mysql/mysql.h>

static MYSQL* mysql = NULL;
static redisContext* redis = NULL;

// todo
// 支持长时间连接
static void initMysql() {
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

static void initRedis() {
    redis = redisConnect("127.0.0.1", 6379);
    // TODO
    if (redis == NULL || redis->err) {
        std::cout << "redis init failed\n";
    }
}

static std::string queryUser(int32_t id) {
    char query_sql[200];
    sprintf(query_sql ,"select username from user_info where id=%d", id);
    std::cout << query_sql << '\n';
    mysql_query(mysql, query_sql);

    MYSQL_RES* res = mysql_store_result(mysql);

    int num_fields = mysql_num_fields(res);
    int num_rows = mysql_num_rows(res);

    MYSQL_ROW row;
    if (row = mysql_fetch_row(res)) {
        std::cout << "find" << row[0] << '\n';
        return row[0];
    }
    else {
        return "";
    }
    
    return "";
}

UserProfileServiceImpl::UserProfileServiceImpl() {
    initMysql();
}

UserProfileServiceImpl::~UserProfileServiceImpl() {
    mysql_close(mysql);
    redisFree(redis);
}

void UserProfileServiceImpl::UserProfile(::google::protobuf::RpcController* cntl_base,
    const UserProfileRequest* request,
    UserProfileResponse* response,
    ::google::protobuf::Closure* done)
{
    brpc::ClosureGuard done_guard(done);
    
    brpc::Controller* cntl = static_cast<brpc::Controller*>(cntl_base);
    
    response->set_username(queryUser(request->id()));
}
