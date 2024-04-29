include(../../../common/Common.cmake)

FILE(GLOB_RECURSE MAIN_SRCS ../main/*)

if(DEFINED ENV{IDF_PATH})
    idf_component_register(SRCS 
                            ${CIOT_ESP32_SRCS}
                            ${MAIN_SRCS}
                        INCLUDE_DIRS
                            ${CIOT_INCLUDE_DIRS}
                            include
                            config
                            .
                    )
else()
    include_directories(
        ${CIOT_INCLUDE_DIRS}
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

endif()
