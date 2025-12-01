add_library(symmetric_algorithms SHARED
    include/feistel_network.hpp
    src/feistel_network.cpp
    include/des.hpp
    src/des.cpp
    include/deal.hpp
    src/deal.cpp
    include/triple_des.hpp
    src/triple_des.cpp
    include/serpent.hpp
    src/serpent.cpp
    include/sp_network.hpp
    src/sp_network.cpp
    include/enc_utility.hpp
    src/enc_utility.cpp
)

target_include_directories(symmetric_algorithms PUBLIC include)

target_link_libraries(symmetric_algorithms 
    symmetric_interface_library
)

add_executable(tests_des
    tests/tests_des.cpp
)

target_link_libraries(tests_des symmetric_algorithms bytes_utility)

add_executable(tests_deal
    tests/tests_deal.cpp
)

target_link_libraries(tests_deal symmetric_algorithms bytes_utility)

add_executable(tests_triple_des
    tests/tests_triple_des.cpp
)

target_link_libraries(tests_triple_des symmetric_algorithms bytes_utility)

add_executable(tests_serpent
    tests/tests_serpent.cpp
)

target_link_libraries(tests_serpent symmetric_algorithms bytes_utility)