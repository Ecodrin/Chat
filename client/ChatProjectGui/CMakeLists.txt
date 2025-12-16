cmake_minimum_required(VERSION 3.16)

project(ChatProject VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools Core Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools Core Sql)


set(TS_FILES ChatProject_ru_RU.ts)

set(PROJECT_SOURCES
        main.cpp
        ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ChatProject
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        registrationwindow.h registrationwindow.cpp registrationwindow.ui
        grpc_client/include/grpc_client.hpp grpc_client/pb/client.grpc.pb.cc grpc_client/pb/client.grpc.pb.h grpc_client/pb/client.pb.cc grpc_client/pb/client.pb.h grpc_client/pb/CMakeLists.txt grpc_client/src/grpc_client.cpp grpc_client/client.proto grpc_client/CMakeLists.txt
        mainwindow.h mainwindow.cpp mainwindow.ui
        contactdialog.h contactdialog.cpp contactdialog.ui
        newconversationdialog.h newconversationdialog.cpp newconversationdialog.ui
        streamgrpcworker.h streamgrpcworker.cpp
        workwithdata.h workwithdata.cpp
        utility.h
        utility.cpp
        chatwidget.h chatwidget.cpp
        clickablewidget.h clickablewidget.cpp
        deletechatdialog.h deletechatdialog.cpp deletechatdialog.ui
        db.h db.cpp
        resources.qrc












    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET ChatProject APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(ChatProject SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(ChatProject
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

add_subdirectory(grpc_client)
add_subdirectory(libs)
target_link_libraries(ChatProject PRIVATE
    grpc_client
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt6::Core
    symmetric_interface_library
    symmetric_algorithms
    Diffie_Hellman_key_exchange
    bytes_utility
    Qt6::Sql
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.ChatProject)
endif()
set_target_properties(ChatProject PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS ChatProject
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ChatProject)
endif()
