#include "login/login.h"

#include <brpc/server.h>
#include <mysql/mysql.h>

static MYSQL* mysql = NULL;

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

static STATUS queryUser(int32_t id, const std::string& password) {
    char query_sql[100];
    sprintf(query_sql ,"select username from user_info where id=%d && password=\'%s\'", id, password.c_str());
    std::cout << query_sql << '\n';
    mysql_query(mysql, query_sql);

    MYSQL_RES* res = mysql_store_result(mysql);
    if (res != NULL) {
        std::cout << mysql_num_rows(res) << '\n';
        if (mysql_num_rows(res) == 0) {
            return STATUS::FAILED;
        }
        else {
            return STATUS::SUCCESS;
        }
    }
    else {
        std::cout << "查询失败: " << mysql_error(mysql) << std::endl;
        return STATUS::FAILED;
    }
    return STATUS::FAILED;
}

static STATUS addUser(const std::string& username, const std::string& password) {
    char add_sql[100];
    sprintf(add_sql, "INSERT INTO user_info (username, password) VALUES (\'%s\', \'%s\')", username.c_str(), password.c_str());
    // TODO 改一下
    if (mysql_query(mysql, add_sql) != 0) {
        return STATUS::FAILED;
    }
    else {
        return STATUS::SUCCESS;
    }
}

LoginServiceImpl::LoginServiceImpl() {
    initMysql();
}

LoginServiceImpl::~LoginServiceImpl() {
    mysql_close(mysql);
    std::cout << "~mysql" << "\n";
}

void LoginServiceImpl::Login(::google::protobuf::RpcController* cntl_base,
            const LoginRequest* request,
            LoginResponse* response,
            ::google::protobuf::Closure* done) {
    
    brpc::ClosureGuard done_guard(done);
    
    brpc::Controller* cntl = static_cast<brpc::Controller*>(cntl_base);
    
    if (request->op_type() == OPERTYPE::LOGIN) {
        std::cout << "LOGIN\n";
        response->set_status(queryUser(request->id(), request->password()));
    }
    else if (request->op_type() == OPERTYPE::REGISTER) {
        std::cout << "REGISTER\n";
        response->set_status(addUser(request->username(), request->password()));
    }
}

