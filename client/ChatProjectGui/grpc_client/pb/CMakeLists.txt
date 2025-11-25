add_library(pb_client 
    client.pb.h
    client.pb.cc
    client.grpc.pb.h
    client.grpc.pb.cc
)

find_package(Protobuf REQUIRED)
target_include_directories(pb_client PUBLIC .)