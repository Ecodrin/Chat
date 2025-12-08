#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <iomanip>
#include <memory>
#include <thread>
#include <format>
#include <tuple>

// для sha256
#include <openssl/sha.h>
#include <openssl/evp.h>

#include <grpcpp/grpcpp.h>
#include "../pb/client.pb.h"
#include "../pb/client.grpc.pb.h"
#include "../../streamgrpcworker.h"
#include "../../workwithdata.h"


using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using chat::Greeter;

struct Contact {
    std::string login;
    int status;
};


struct ChatSessionCallResult {
    std::unique_ptr<grpc::ClientReaderWriter<chat::ChatMsg, chat::ChatMsg>> writer;
    std::string error;
};


class GreeterClient {
public:
    GreeterClient(std::shared_ptr<Channel> channel);
    std::pair<bool, std::string> registration(const std::string & login, const std::string& password);
    std::pair<bool, std::string> auth(const std::string & login, const std::string& password);
    std::pair<bool, std::string> disconnect();
    std::pair<std::vector<Contact>, std::string> get_all_contacts() const;
    std::pair<bool, std::string> accept_contact(const std::string & contact) const;
    std::pair<bool, std::string> decline_contact(const std::string & contact) const;
    std::pair<bool, std::string> add_contact(const std::string & contact) const;
    std::pair<bool, std::string> delete_contact(const std::string & contact) const;
    ChatSessionCallResult chat_session(grpc::ClientContext *context) const;

    std::pair<bool, std::string> add_chat(std::shared_ptr<ChatStreamgRPCWorker> writer, ChatInfo chat_info) const;
    std::pair<bool, std::string> send_msg(std::shared_ptr<ChatStreamgRPCWorker> writer, MsgData msg_data) const;
    std::pair<bool, std::string> send_file(std::shared_ptr<ChatStreamgRPCWorker> writer, FileData msg_data) const;
    std::pair<bool, std::string> delete_chat(
            std::shared_ptr<ChatStreamgRPCWorker> writer,
            const std::string & chat_id,
            const std::string & recipient) const;

    std::string get_login() const;

private:
    std::string token;
    std::string login;

    static std::pair<bool, std::string> validate_login_and_password(const std::string & login, const std::string& password);
    static std::pair<bool, std::string> validate_login(const std::string & login);
    std::unique_ptr<Greeter::Stub> stub_;

};

std::string sha256(const std::string& str) ;

