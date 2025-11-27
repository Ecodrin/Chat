#ifndef STREAMGRPCWORKER_H
#define STREAMGRPCWORKER_H


#include <grpcpp/grpcpp.h>
#include <functional>
#include <memory>
#include <memory>
#include <mutex>
#include <thread>

#include "client.grpc.pb.h"
#include "client.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using chat::Greeter;

class ChatStreamgRPCWorker
{
public:
    using CallBack = std::function<void(const chat::ChatMsg &)>;
    int write(const chat::ChatMsg & msg);


    ChatStreamgRPCWorker(std::unique_ptr<grpc::ClientReaderWriter<chat::ChatMsg, chat::ChatMsg>> stream, CallBack callback);
    ~ChatStreamgRPCWorker();
private:
    void start();

    std::unique_ptr<grpc::ClientReaderWriter<chat::ChatMsg, chat::ChatMsg>> stream;
    grpc::CompletionQueue cq;
    std::mutex mutex;
    std::thread read_t;
    CallBack callback;
};

#endif // STREAMGRPCWORKER_H
