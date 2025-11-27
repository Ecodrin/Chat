add_library(symmetric_interface_library SHARED
    include/symmetric_encryption_interfaces.hpp
    src/symmetric_encryption_interfaces.cpp
    include/symmetric_context.hpp
    src/symmetric_context.cpp
    include/encryption_modes.hpp
    src/encryption_modes.cpp
    include/encryption_padding.hpp
    src/encryption_padding.cpp

)

target_include_directories(symmetric_interface_library PUBLIC include)

target_link_libraries(symmetric_interface_library
    bytes_utility
)

# add_executable(tests_symmetric_interface_library tests/tests_symmetric_interface_library.cpp)

# target_link_libraries(symmetric_interface_library
#     bytes_utility
# )

# target_link_libraries(tests_symmetric_interface_library
#     symmetric_interface_library
# )
