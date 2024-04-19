include(../../../common/Common.cmake)

FILE(GLOB_RECURSE MAIN_SRCS ../main/*)

include_directories(
    ${CIOT_INCLUDE_DIRS}
    ${PROTO_INCLUDE_DIRS}
    include
    config
    .
)

add_executable(${PROJECT_NAME}
    ${CIOT_PC_SRCS}
    ${MAIN_SRCS}
)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} ws2_32)
endif()
