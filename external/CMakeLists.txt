add_library(crow INTERFACE)
target_include_directories(crow INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/crow")
target_link_libraries(crow INTERFACE ${Boost_LIBRARIES})

add_subdirectory(openssl)

add_library(httplib INTERFACE)
target_include_directories(httplib INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/httplib")
target_link_libraries(httplib INTERFACE ssl crypto)