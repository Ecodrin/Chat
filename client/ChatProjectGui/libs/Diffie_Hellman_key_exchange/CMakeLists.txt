add_library(diffie_hellman_key_exchange SHARED
    include/diffie_hellman_key_exchange.hpp
    src/diffie_hellman_key_exchange.cpp
)

target_include_directories(diffie_hellman_key_exchange PUBLIC include)

target_link_libraries(diffie_hellman_key_exchange 
    Boost::random 
    Boost::system
    bytes_utility
    asymmetric_algorithms
)


add_executable(tests_diffie_hellman_key_exchange
    tests/tests_diffie_hellman_key_exchange.cpp
)

target_link_libraries(tests_diffie_hellman_key_exchange
    diffie_hellman_key_exchange
)