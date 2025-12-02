#include "../include/grpc_client.hpp"


GreeterClient::GreeterClient(std::shared_ptr<Channel> channel): stub_(Greeter::NewStub(channel)) {

}

std::pair<bool, std::string> GreeterClient::registration(const std::string & login, const std::string& password) {
    auto val = validate_login_and_password(login, password);
    if (!val.first) {
        return val;
    }
    chat::RegistrationRequest request;
    request.set_login(login);
    request.set_password(sha256(password));
    
    chat::StatusRegistrationAuthResponse reply;
    grpc::ClientContext context;

    grpc::Status status = stub_->Registration(&context, request, &reply);
    if (status.ok()) {
        token = reply.token();
        this->login = login;
        return {true, ""};
    }
    else {
        if(status.error_code() == grpc::StatusCode::UNAVAILABLE) {
            return {false, "failed connection"};
        }
        return {false, status.error_message()};
    }
}


std::pair<bool, std::string> GreeterClient::auth(const std::string & login, const std::string& password) {
    auto val = validate_login_and_password(login, password);
    if (!val.first) {
        return val;
    }
    chat::AuthRequest request;
    request.set_login(login);
    request.set_password(sha256(password));

    chat::StatusRegistrationAuthResponse reply;
    grpc::ClientContext context;

    grpc::Status status = stub_->Auth(&context, request, &reply);
    if (status.ok()) {
        token = reply.token();
        this->login = login;
        return {true, ""};
    }
    else {
        if(status.error_code() == grpc::StatusCode::UNAVAILABLE) {
            return {false, "failed connection"};
        }
        return {false, status.error_message()};
    }
}

std::pair<bool, std::string> GreeterClient::disconnect() {
    chat::EmptyMsg request;
    chat::StatusResponse response;
    grpc::ClientContext context;
    context.AddMetadata("authorization", token);
    grpc::Status status = stub_->Disconnect(&context, request, &response);
    token = "";
    login = "";
    if(status.ok()) {
        token = "";
        return {true, ""};
    } else {
        if(status.error_code() == grpc::StatusCode::UNAVAILABLE) {
            return {false, "failed connection"};
        }
        return {false, status.error_message()};
    }

}

std::string sha256(const std::string& input) {
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hashLen;
    
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_sha256(), NULL);
    EVP_DigestUpdate(ctx, input.c_str(), input.length());
    EVP_DigestFinal_ex(ctx, hash, &hashLen);
    EVP_MD_CTX_free(ctx);
    
    std::string result;
    for (unsigned int i = 0; i < hashLen; i++) {
        result += std::format("{:02x}", hash[i]);
    }
    return result;
}

std::pair<bool, std::string> GreeterClient::validate_login(const std::string & login) {
    if (login.size() < 3 || login.size() > 50) {
        return {false, "login len must be >= 3 and < 50"};
    }
    bool fl = false;

    for (const auto & c : login) {
        if (!(std::isalpha(c) || (c >= '0' && c <='9'))) {
            return {false, "incorrect character in login"};
        }
    }
    return {true, ""};
}

std::pair<bool, std::string> GreeterClient::validate_login_and_password(const std::string & login, const std::string& password) {
    if (login.size() < 3 || login.size() > 50) {
        return {false, "login len must be >= 3 and < 50"};
    }
    bool fl = false;

    for (const auto & c : login) {
        if (!(std::isalpha(c) || (c >= '0' && c <='9'))) {
            return {false, "incorrect character in login"};
        }
    }
    if (password.size() < 15) {
        return {false, "password len must be >= 15"};
    }   
    return {true, ""};
}

std::pair<std::vector<Contact>, std::string> GreeterClient::get_all_contacts() const {
    grpc::ClientContext context;
    context.AddMetadata("authorization", token);
    chat::EmptyMsg request;
    chat::GetContactsResponse response;
    grpc::Status status = stub_->GetContacts(&context, request, &response);
    if(status.ok()) {
        std::vector<Contact> contacts;
        for(size_t i = 0; i < response.statuses().size(); ++i) {
            contacts.emplace_back(Contact{response.contats()[i], static_cast<int>(response.statuses()[i])});
        }
        return {contacts, ""};
    } else {
        if(status.error_code() == grpc::StatusCode::UNAVAILABLE) {
            return {{}, "failed connection"};
        }
        return {{}, status.error_message()};
    }
}

std::pair<bool, std::string> GreeterClient::accept_contact(const std::string & contact) const {
    grpc::ClientContext context;
    context.AddMetadata("authorization", token);
    chat::NewContactRequest request;
    request.set_contact(contact);
    chat::StatusResponse response;
    grpc::Status status = stub_->AcceptRequestContact(&context, request, &response);
    if (status.ok()) {
        return {true, ""};
    } else {
        if(status.error_code() == grpc::StatusCode::UNAVAILABLE) {
            return {false, "failed connection"};
        }
        return {false, status.error_message()};
    }
}


std::pair<bool, std::string> GreeterClient::decline_contact(const std::string & contact) const {
    grpc::ClientContext context;
    context.AddMetadata("authorization", token);
    chat::NewContactRequest request;
    request.set_contact(contact);
    chat::StatusResponse response;
    grpc::Status status = stub_->DeclineRequestContact(&context, request, &response);
    if (status.ok()) {
        return {true, ""};
    } else {
        if(status.error_code() == grpc::StatusCode::UNAVAILABLE) {
            return {false, "failed connection"};
        }
        return {false, status.error_message()};
    }
}

std::pair<bool, std::string> GreeterClient::add_contact(const std::string & contact) const {
    auto t = validate_login(contact);
    if(!t.first) {
        return t;
    }
    grpc::ClientContext context;
    context.AddMetadata("authorization", token);
    chat::NewContactRequest request;
    request.set_contact(contact);
    chat::StatusResponse response;
    grpc::Status status = stub_->AddContact(&context, request, &response);
    if (status.ok()) {
        return {true, ""};
    } else {
        if(status.error_code() == grpc::StatusCode::UNAVAILABLE) {
            return {false, "failed connection"};
        }
        return {false, status.error_message()};
    }
}


std::pair<bool, std::string> GreeterClient::delete_contact(const std::string & contact) const {
    auto t = validate_login(contact);
    if(!t.first) {
        return t;
    }
    grpc::ClientContext context;
    context.AddMetadata("authorization", token);
    chat::DeleteContactRequest request;
    request.set_contact(contact);
    chat::StatusResponse response;
    grpc::Status status = stub_->DeleteContact(&context, request, &response);
    if (status.ok()) {
        return {true, ""};
    } else {
        if(status.error_code() == grpc::StatusCode::UNAVAILABLE) {
            return {false, "failed connection"};
        }
        return {false, status.error_message()};
    }
}

ChatSessionCallResult GreeterClient::chat_session(grpc::ClientContext *context) const {
    context->AddMetadata("authorization", token);
    auto msgs_writer = stub_->ChatSession(context);
    if (msgs_writer == nullptr) {
        return ChatSessionCallResult{std::unique_ptr<grpc::ClientReaderWriter<chat::ChatMsg, chat::ChatMsg>>{nullptr}, "bad token error"};
    }

    return ChatSessionCallResult{std::move(msgs_writer), ""};
}

std::pair<bool, std::string> GreeterClient::add_chat(std::shared_ptr<ChatStreamgRPCWorker> writer, ChatInfo chat_info) const {
    grpc::ClientContext context;
    context.AddMetadata("authorization", token);

    chat::ChatMsg chat_msg;
    chat::NewChatMsg *new_chat_msg = chat_msg.mutable_new_chat_msg();
    new_chat_msg->set_chat_id(chat_info.chat_id);
    new_chat_msg->set_sender(login);
    new_chat_msg->set_recipient(chat_info.interlocutor);
    new_chat_msg->set_ab_for_key(chat_info.ab_key);
    new_chat_msg->set_g_for_key(chat_info.g_key);
    new_chat_msg->set_p_for_key(chat_info.p_key);
    new_chat_msg->set_ab_for_iv({chat_info.ab_iv});
    new_chat_msg->set_g_for_iv(chat_info.g_iv);
    new_chat_msg->set_p_for_iv(chat_info.p_iv);
    if(writer->write(chat_msg)) {
        return {false, "error in send msg"};
    }
    return {true, ""};
}


