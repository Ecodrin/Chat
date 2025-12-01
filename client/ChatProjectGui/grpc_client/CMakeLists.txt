add_library(grpc_client
    include/grpc_client.hpp
    src/grpc_client.cpp
)

find_package(gRPC CONFIG REQUIRED)

find_package(OpenSSL REQUIRED)


add_subdirectory(pb)
target_link_libraries(grpc_client 
    pb_client 
    OpenSSL::SSL 
    OpenSSL::Crypto
    gRPC::grpc++
    gRPC::grpc
    protobuf::libprotobuf
    pthread
    symmetric_interface_library
    Diffie_Hellman_key_exchange
    symmetric_algorithms
)

target_include_directories(grpc_client PUBLIC include)
