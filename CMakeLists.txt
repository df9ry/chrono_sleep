cmake_minimum_required(VERSION 3.10)
project(chrono_sleep
    VERSION     1.0.0
    DESCRIPTION "Sleep tool using c++ chrono library")

set(SOURCES
    main.cpp
    getopt.c
)

set(HEADERS
    getopt.h
    version.hpp.in
)

set(APP_EXE chrono_sleep)
add_executable(${APP_EXE} ${SOURCES} ${HEADERS})
configure_file(version.hpp.in version.hpp)

target_compile_features(${APP_EXE}
    PUBLIC cxx_std_20)
target_include_directories(${APP_EXE} PUBLIC
    BEFORE "${CMAKE_INSTALL_PREFIX}/${CMAKE_BUILD_TYPE}/include/"
    BEFORE "${PROJECT_BINARY_DIR}")
target_link_directories(${APP_EXE} PUBLIC
    BEFORE "${CMAKE_INSTALL_PREFIX}/${CMAKE_BUILD_TYPE}/lib/")
if (WIN32)
    target_link_libraries(${APP_EXE}
)
else()
    target_link_libraries(${APP_EXE}
        stdc++
)
endif()

install(TARGETS ${APP_EXE}
    RUNTIME  DESTINATION "${CMAKE_BUILD_TYPE}/bin")
