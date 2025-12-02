get_filename_component(PROJECT_DIR "${CMAKE_CURRENT_LIST_FILE}/../../../.." ABSOLUTE)

include(${PROJECT_DIR}/common/Common.cmake)

FILE(GLOB_RECURSE MAIN_SRCS *.c)

if(DEFINED ENV{IDF_PATH})
    idf_build_get_property(target IDF_TARGET)
    idf_component_register(
        SRCS
            ${CIOT_SRCS}
            ${CIOT_PB_V2_SRCS}
            ${CIOT_ESP32_SRCS}
            ${CIOT_MBUS_SRCS}
            ${MAIN_SRCS}
        INCLUDE_DIRS
            .
            ${CIOT_INCLUDE_DIRS}
            ${CIOT_PB_INCLUDE_DIRS}
            ${CIOT_MBUS_INCLUDE_DIRS}
    )
else()
    include_directories(
        .
        ${CIOT_INCLUDE_DIRS}
        ${CIOT_PB_INCLUDE_DIRS}
        ${CIOT_MBUS_INCLUDE_DIRS}
    )
    add_executable(${PROJECT_NAME}
        ${CIOT_SRCS}
        ${CIOT_PB_V2_SRCS}
        ${CIOT_MG_SRCS}
        ${CIOT_WIN_SRCS}
        ${CIOT_MBUS_SRCS}
        ${MAIN_SRCS}
    )
    if(WIN32)
        target_link_libraries(${PROJECT_NAME} iphlpapi ws2_32 bcrypt)
    endif()
endif()
