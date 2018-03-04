cmake_minimum_required(VERSION 3.10)
project(sofutobanku)
set(MAJOR_VERSION 1)
set(MINOR_VERSION 0)
set(PACKAGE sofutobanku)
set(VERSION_STRING ${MAJOR_VERSION}.${MINOR_VERSION})
set(VERSION ${VERSION_STRING})

# We require C++11 because of unique_ptr, et al.
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(SBRadLib "")
add_executable(sbradclient "")
add_executable(sbradclient_tests "")
include(src/CMakeLists.txt)

#
# Make /usr the default install prefix
#
SET(SOFUTOBANKU_INSTALL_PREFIX "/usr" CACHE PATH "Prefix prepended to install directories")
SET(CMAKE_INSTALL_PREFIX "${SOFUTOBANKU_INSTALL_PREFIX}" CACHE INTERNAL "Prefix prepended to install directories" FORCE)

#
# Replace directory variables in scripts
#
SET(_default_sysconf_dir "etc")
IF(CMAKE_INSTALL_PREFIX STREQUAL "/usr")
  SET(_default_sysconf_dir "/etc")
  SET(CPACK_PACKAGE_RELOCATABLE FALSE)
ENDIF()
SET(CMAKE_INSTALL_SYSCONFDIR "${_default_sysconf_dir}")

SET(PREFIX "${CMAKE_INSTALL_PREFIX}")
SET(_instdir "${PROJECT_BINARY_DIR}/instfiles")
FILE(REMOVE_RECURSE "${_instdir}")
SET(_etcdir "${_instdir}/etc")
FILE(MAKE_DIRECTORY "${_etcdir}")
CONFIGURE_FILE("${CMAKE_CURRENT_LIST_DIR}/etc/dibbler.sh.in" "${_etcdir}/dibbler.sh" @ONLY)
INSTALL(FILES "${_etcdir}/dibbler.sh"
	PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
       	DESTINATION "${CMAKE_INSTALL_SYSCONFDIR}/${PROJECT_NAME}")

CONFIGURE_FILE("${PROJECT_SOURCE_DIR}/src/config.h.in"
	       "${PROJECT_BINARY_DIR}/config.h")
INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR})

#
# Include nettle for MD5 calculation
#
find_package(PkgConfig REQUIRED)
pkg_search_module(NETTLE REQUIRED nettle)

if(NETTLE_FOUND)
	include_directories(${NETTLE_INCLUDE_DIRS})
	message(STATUS "Using nettle ${NETTLE_VERSION}")
else()
	# We should not reach here since nettle is REQUIRED
endif()


#
# Include radcli for RADIUS client
#
pkg_search_module(RADCLI REQUIRED radcli)

if(RADCLI_FOUND)
	include_directories(${RADCLI_INCLUDE_DIRS})
	message(STATUS "Using radcli ${RADCLI_VERSION}")
else()
	# We should not get here since radcli is REQUIRED
endif()

target_link_libraries(SBRadLib ${NETTLE_LIBRARIES} ${RADCLI_LIBRARIES})

INSTALL(TARGETS sbradclient DESTINATION sbin)
INSTALL(FILES etc/dictionary.softbank DESTINATION "${CMAKE_INSTALL_SYSCONFDIR}/${PROJECT_NAME}")


#
# GoogleTest is used for testing functions
#

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


#
# Setup testing
#
enable_testing()
include(GoogleTest)
gtest_discover_tests(sbradclient_tests)


#
# Packaging instructions
#
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION ${VERSION_STRING})
set(CPACK_PACKAGE_RELEASE 1)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_LIST_DIR}/LICENSE")
set(CPACK_PACKAGE_VENDOR "Kenny Root")
set(CPACK_PACKAGE_CONTACT "https://github.com/kruton/sofutobanku/")
set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
set(CPACK_RPM_PACKAGE_DESCRIPTION "Sets up Internet connections for Softbank Hikari")
set(CPACK_RPM_PACKAGE_URL "https://github.com/kruton/sofutobanku/")
set(CPACK_RPM_PACKAGE_LICENSE "ASL 2.0")
set(CPACK_RPM_PACKAGE_GROUP "System Environment/Base")
set(CPACK_RPM_CHANGELOG_FILE "${CMAKE_CURRENT_LIST_DIR}/ChangeLog.txt")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_PACKAGE_RELEASE}.${CMAKE_SYSTEM_PROCESSOR}")
LIST(APPEND CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "/usr/sbin")
LIST(APPEND CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "/usr/local")
set(CPACK_GENERATOR "RPM;DEB")
include(CPack)
