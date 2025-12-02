find_package(Boost REQUIRED COMPONENTS filesystem random system)

add_library(asymmetric_algorithms SHARED
    include/symbol_service.hpp
    src/symbol_service.cpp
    include/primality_tests.hpp
    src/primality_tests.cpp
    include/rsa.hpp
    src/rsa.cpp
    include/wieners_attack.hpp
    src/wieners_attack.cpp
)



target_link_libraries(asymmetric_algorithms 
    Boost::random
    Boost::system
    bytes_utility
)

target_include_directories(asymmetric_algorithms PUBLIC include)

# add_executable(tests_generate_symbols 
#     tests/tests_generate_symbols.cpp
# )

# target_link_libraries(tests_generate_symbols asymmetric_algorithms)

# add_executable(tests_primality_tests
#     tests/tests_primality_tests.cpp
# )

# target_link_libraries(tests_primality_tests asymmetric_algorithms)

add_executable(tests_rsa
    tests/tests_rsa.cpp
)

target_link_libraries(tests_rsa 
    asymmetric_algorithms
    bytes_utility
)

add_executable(tests_wieners_attack
    tests/tests_wieners_attack.cpp
)

target_link_libraries(tests_wieners_attack
    asymmetric_algorithms
)