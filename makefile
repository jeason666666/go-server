CXX=g++

GATEWAY_SRC:=$(shell find ./src/httpgateway -name "*.cc" -o -name "*.cpp")
LOGIN_SRC:=$(shell find ./src/login -name "*.cc" -o -name "*.cpp")
USER_SRC:=$(shell find ./src/user_profile -name "*.cc" -o -name "*.cpp")
FRI_SRC:=$(shell find ./src/friend_manage -name "*.cc" -o -name "*.cpp")
MES_SRC:=$(shell find ./src/message_manage -name "*.cc" -o -name "*.cpp")
MT_SRC:=$(shell find ./src/message_transmit -name "*.cc" -o -name "*.cpp")

GATEWAY_OBJ:=$(addsuffix .o, $(basename $(GATEWAY_SRC))) 
LOGIN_OBJ:=$(addsuffix .o, $(basename $(LOGIN_SRC))) 
USER_OBJ:=$(addsuffix .o, $(basename $(USER_SRC))) 
FRI_OBJ:=$(addsuffix .o, $(basename $(FRI_SRC)))
MES_OBJ:=$(addsuffix .o, $(basename $(MES_SRC)))
MT_OBJ:=$(addsuffix .o, $(basename $(MT_SRC)))

CXX_FALGS=-std=c++0x -DNDEBUG -O0 -pipe -W -Wall -Wno-unused-parameter -fPIC -fno-omit-frame-pointer -fno-tree-vectorize
# CXX_FALGS=-std=c++0x

LD_FLAGS = -L/home/ljs/go-server/lib/thirdlib

MACRO= -DBRPC_WITH_GLOG=0 -DGFLAGS_NS=google -DBRPC_DEBUG_BTHREAD_SCHE_SAFETY=0 -DBRPC_DEBUG_LOCK=0 -D__const__=__unused__ -g

DYNAMIC_LINKINGS=-Wl,-Bdynamic -lbrpc -lprotobuf -lgflags -lpthread -lssl -lcrypto -ldl -lz -lrt -lleveldb -lmysqlclient -lhiredis -Wl,-rpath=./lib/thirdlib

INCLUDE_BRPC=-I./lib/thirdlib/brpc/output/include
INCLUDE_SPDLOG=-I./lib/thirdlib/spdlog/include
INCLUDE_PATH=-I./lib/thirdlib/brpc/output/include -I./src -I./lib/thirdlib/json -I./lib/thirdlib/spdlog/include

%.o:%.cc
	$(CXX) $(CXX_FALGS) $(MACRO) $(INCLUDE_PATH) -c $^ -o $@

%.o:%.cpp
	$(CXX) $(CXX_FALGS) $(MACRO) $(INCLUDE_PATH) -c $^ -o $@

all:ma l u f me mt
ma:main_server
l:login_server
u:user_profile_server
f:friend_manage_server
me:message_manage_server
mt:message_transmit_server

# 网关微服务
main_server:$(GATEWAY_OBJ) $(LOGIN_OBJ) $(USER_OBJ) $(FRI_OBJ) $(MES_OBJ) $(MT_OBJ) src/main_server.cpp
	$(CXX) $^ $(CXX_FALGS) $(MACRO) $(LD_FLAGS) $(DYNAMIC_LINKINGS) $(INCLUDE_PATH) -o main

# 登陆和注册
login_server:$(LOGIN_OBJ) src/login_server.cpp
	$(CXX) $^ $(CXX_FALGS) $(MACRO) $(LD_FLAGS) $(DYNAMIC_LINKINGS) $(INCLUDE_PATH) -o login

# 用户信息获取
user_profile_server:$(USER_OBJ) src/user_profile_server.cpp
	$(CXX) $^ $(CXX_FALGS) $(MACRO) $(LD_FLAGS) $(DYNAMIC_LINKINGS) $(INCLUDE_PATH) -o user_profile

# 添加好友 查询好友申请 好友列表查询
friend_manage_server:$(FRI_OBJ) $(USER_OBJ) src/friend_manage_server.cpp
	$(CXX) $^ $(CXX_FALGS) $(MACRO) $(LD_FLAGS) $(DYNAMIC_LINKINGS) $(INCLUDE_PATH) -o friend_manage

# 添加用户历史消息记录 获取用户历史消息记录（客户端本地需要存储）
# 读写量大
message_manage_server:$(MES_OBJ) $(MT_OBJ) src/message_manage_server.cpp
	$(CXX) $^ $(CXX_FALGS) $(MACRO) $(LD_FLAGS) $(DYNAMIC_LINKINGS) $(INCLUDE_PATH) -o message_manage


message_transmit_server:$(MT_OBJ) src/message_transmit_server.cpp
	$(CXX) $^ $(CXX_FALGS) $(MACRO) $(LD_FLAGS) $(DYNAMIC_LINKINGS) $(INCLUDE_PATH) -o message_transmit

run_all:
	./main & ./login & ./user_profile & ./friend_manage & ./message_manage & ./message_transmit &

stop_all:
	jobs -p | xargs kill

count:
	wc $(GATEWAY_SRC) $(LOGIN_SRC) $(USER_SRC) $(FRI_SRC) $(MES_SRC) src/login_server.cpp src/main_server.cpp src/user_profile_server.cpp -l

clean:
	rm -rf main login user_profile friend_manage message_manage message_transmit $(USER_OBJ) $(LOGIN_OBJ) $(GATEWAY_OBJ) $(FRI_OBJ) $(MES_OBJ) $(MT_OBJ)

run:
	./main
