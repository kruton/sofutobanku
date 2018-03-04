target_sources(SBRadLib
    PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/ip_address.cc"
        "${CMAKE_CURRENT_LIST_DIR}/radius.cc"
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/ip_address.h"
        "${CMAKE_CURRENT_LIST_DIR}/radius.h"
        "${CMAKE_CURRENT_LIST_DIR}/unique_types.h"
)

target_include_directories(SBRadLib PUBLIC "${CMAKE_CURRENT_LIST_DIR}")

target_sources(sbradclient
    PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/main.cc"
)

target_link_libraries(sbradclient PUBLIC SBRadLib)

target_sources(sbradclient_tests
    PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/ip_address_tests.cc"
)

target_link_libraries(sbradclient_tests SBRadLib gtest_main)
