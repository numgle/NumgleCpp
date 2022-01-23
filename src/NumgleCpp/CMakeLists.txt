set(SOURCE_LIST
    NumgleApp.h
    DatasetRepository.h
    UTF8Decoder.h
    main.cpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_LIST})
add_executable(NumgleCpp ${SOURCE_LIST}) 

if(MSVC AND NOT (MSVC_VERSION LESS 1900))
    target_link_libraries(NumgleCpp PUBLIC crow httplib "legacy_stdio_definitions.lib")

else()
    target_link_libraries(NumgleCpp PUBLIC crow httplib)
endif()

