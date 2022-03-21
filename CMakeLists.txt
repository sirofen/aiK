cmake_minimum_required(VERSION 3.16)

project(ai_k_widg VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH $ENV{QT_MSVC_CMAKE_PATH})

option(HIDE_CONSOLE_WINDOW "Hide console window" OFF)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

set(PROJECT_SOURCES
        main.cpp
        aikwidget.cpp
        aikwidget.h
        aikwidget.ui
        collapse_button.h

        style/style.hpp
        style/style.cpp

        logview.hpp
        logview.cpp

        aik/symbols/aik_process.hpp
        aik/symbols/client_values.hpp
        aik/symbols/aion_write_builder.hpp
        aik/symbols/aion_write.hpp

        aik/err.hpp

        aik_control/aik_worker.hpp
        aik_control/aik_worker.cpp
        aik_control/aik_process_values.hpp
        aik_control/aik_process_values.cpp
        aik_control/aik_write_instructions.hpp
        aik_control/aik_write_instructions.cpp

        wintaskscheduler.hpp
        wintaskscheduler.cpp

        lock_mod.hpp
)

qt_add_resources(PROJECT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/aiK.qrc)
set(PROJECT_SOURCES ${PROJECT_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/app.rc)

if (${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ai_k_widg
            MANUAL_FINALIZATION
            ${PROJECT_SOURCES}
            )
else ()
    add_executable(ai_k_widg
            ${PROJECT_SOURCES}
            )
endif ()


if (HIDE_CONSOLE_WINDOW)
set_target_properties(ai_k_widg PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

# download aik-user
include(cmake/FetchAionUser.cmake)

target_link_libraries(ai_k_widg PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(ai_k_widg PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

# POST_BUILD patcher
include(cmake/PatchBinary.cmake)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ai_k_widg)
endif()

