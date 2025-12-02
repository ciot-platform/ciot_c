# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly

cmake_minimum_required(VERSION 3.10...3.25)

include(../common/Common.cmake)

project(tests)

add_library(unity STATIC unity/src/unity.c)

FILE(GLOB_RECURSE COMMON_TESTS common/*)

include_directories(
    ${CIOT_INCLUDE_DIRS}
    ${CIOT_PB_INCLUDE_DIRS}
    unity/src
    .
)

add_executable(${PROJECT_NAME}
    ${CIOT_SRCS}
    ${CIOT_PB_V2_SRCS}
    ${CIOT_MG_SRCS}
    ${CIOT_WIN_SRCS}
    ${MAIN_SRCS}
    ${COMMON_TESTS}
    unity/src/unity.c
    main.c
)

target_link_libraries(${PROJECT_NAME} unity)

if (WIN32)
    target_link_libraries(${PROJECT_NAME} ws2_32)
    target_link_libraries(${PROJECT_NAME} ws2_32 bcrypt)
endif()
