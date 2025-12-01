find_package(Boost REQUIRED COMPONENTS filesystem random system)

add_library(Diffie_Hellman_key_exchange SHARED
    include/Diffie_Hellman_key_exchange.hpp
    src/Diffie_Hellman_key_exchange.cpp
)

target_include_directories(Diffie_Hellman_key_exchange PUBLIC include)

target_link_libraries(Diffie_Hellman_key_exchange 
    Boost::random 
    Boost::system
    bytes_utility
    asymmetric_algorithms
)


add_executable(tests_Diffie_Hellman_key_exchange
    tests/tests_Diffie_Hellman_key_exchange.cpp
)

target_link_libraries(tests_Diffie_Hellman_key_exchange
    Diffie_Hellman_key_exchange
)