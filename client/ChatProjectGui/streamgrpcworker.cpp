#include "streamgrpcworker.h"

ChatStreamgRPCWorker::ChatStreamgRPCWorker(std::unique_ptr<grpc::ClientReaderWriter<chat::ChatMsg, chat::ChatMsg>> stream, CallBack callback) :
    stream{std::move(stream)},
    callback{callback} {
    start();
}

ChatStreamgRPCWorker::~ChatStreamgRPCWorker() {
    if(read_t.joinable()) {
        read_t.join();
    }
}


void ChatStreamgRPCWorker::start() {
    read_t = std::thread([this]() {
        chat::ChatMsg msg;
        while(stream && stream->Read(&msg)) {
            callback(msg);
        }
    });
}

int ChatStreamgRPCWorker::write(const chat::ChatMsg & msg) {
    if(!stream) {
        return 1;
    }
    bool fl = stream->Write(msg);
    if(!fl) {
        return 1;
    }
    return 0;
}


