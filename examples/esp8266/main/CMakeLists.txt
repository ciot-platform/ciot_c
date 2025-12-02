get_filename_component(PROJECT_DIR "${CMAKE_CURRENT_LIST_FILE}/../../../.." ABSOLUTE)

include(${PROJECT_DIR}/common/Common.cmake)

FILE(GLOB_RECURSE MAIN_SRCS *.c)

idf_component_register(
    SRCS
        ${MAIN_SRCS}
        ${CIOT_SRCS}
        ${CIOT_ESP32_SRCS}
        ${CIOT_PB_V2_SRCS}
    INCLUDE_DIRS
        .
        ${CIOT_INCLUDE_DIRS}
        ${CIOT_PB_INCLUDE_DIRS}
)