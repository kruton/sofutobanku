cmake_minimum_required (VERSION 2.6)
project (SBRadClient)

add_library (SBRadLib radius.cc radius.h ip_address.cc ip_address.h)
add_executable (sbradclient main.cc)
target_link_libraries (sbradclient SBRadLib)

find_package (PkgConfig REQUIRED)
pkg_search_module(OPENSSL REQUIRED openssl)

if (OPENSSL_FOUND)
	include_directories (${OPENSSL_INCLUDE_DIRS})
	message (STATUS "Using OpenSSL ${OPENSSL_VERSION}")
else ()
	# We should not reach here since OpenSSL is REQUIRED
endif ()

pkg_search_module (RADCLI REQUIRED radcli)

if (RADCLI_FOUND)
	include_directories (${RADCLI_INCLUDE_DIRS})
	message (STATUS "Using radcli ${RADCLI_VERSION}")
else ()
	# We should not get here since radcli is REQUIRED
endif ()

target_link_libraries (SBRadLib ${OPENSSL_LIBRARIES} ${RADCLI_LIBRARIES})

install (TARGETS sbradclient DESTINATION bin)
install (FILES softbank.dictionary DESTINATION etc)

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
                 ${CMAKE_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

enable_testing ()
include (GoogleTest)

add_executable(sbradclient_tests ip_address_tests.cc)
target_link_libraries(sbradclient_tests SBRadLib gtest_main)

gtest_discover_tests(sbradclient_tests)
