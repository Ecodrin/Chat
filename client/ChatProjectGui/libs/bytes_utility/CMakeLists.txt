add_library(bytes_utility SHARED
    include/bytes_utility.hpp
    src/bytes_utility.cpp
)

target_include_directories(bytes_utility PUBLIC 
    include
)

add_executable(tests_bytes_utility 
    tests/tests_bytes_utility.cpp
)



target_link_libraries(tests_bytes_utility bytes_utility)
