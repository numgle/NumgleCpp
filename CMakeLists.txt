cmake_minimum_required(VERSION 3.13.4)
project(NumgleCpp)

macro(boost_configure)
	if (WIN32)
		execute_process(
			COMMAND ${BOOST_SOURCEDIR}/bootstrap.bat
			WORKING_DIRECTORY ${BOOST_SOURCEDIR}
		)
	elseif(UNIX)
		execute_process(
			COMMAND chmod +x ${BOOST_SOURCEDIR}/tools/build/src/engine/build.sh
			COMMAND sh ${BOOST_SOURCEDIR}/bootstrap.sh
			WORKING_DIRECTORY ${BOOST_SOURCEDIR}
		)
	endif()
endmacro(boost_configure)

macro(boost_compile)
	if (CMAKE_SYSTEM_NAME STREQUAL Windows)
		execute_process(
			COMMAND ${BOOST_SOURCEDIR}/b2 --build-dir=${BOOST_INSTALLDIR} --stagedir=${BOOST_INSTALLDIR} toolset=msvc stage
			WORKING_DIRECTORY ${BOOST_SOURCEDIR}
		)
	elseif(CMAKE_SYSTEM_NAME STREQUAL Darwin)
		execute_process(
			COMMAND ${BOOST_SOURCEDIR}/b2 --build-dir=${BOOST_INSTALLDIR} --stagedir=${BOOST_INSTALLDIR} stage
			WORKING_DIRECTORY ${BOOST_SOURCEDIR}
		)
	elseif(CMAKE_SYSTEM_NAME STREQUAL Linux)
		execute_process(
			COMMAND ${BOOST_SOURCEDIR}/b2 --ignore-site-config --build-dir=${BOOST_INSTALLDIR} --stagedir=${BOOST_INSTALLDIR} stage
			WORKING_DIRECTORY ${BOOST_SOURCEDIR}
		)
	endif()
endmacro(boost_compile)

set(BOOST_SOURCEDIR "${CMAKE_SOURCE_DIR}/external/ext-boost/")
set(BOOST_INSTALLDIR "${CMAKE_BINARY_DIR}/external/ext-boost/")

unset(b2 CACHE)

if (${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
    find_file(
        b2
        b2.exe
        PATHS "${BOOST_SOURCEDIR}"
        NO_CMAKE_PATH
    )
else()
    find_file(
        b2
        b2
        PATHS "${BOOST_SOURCEDIR}"
    )
endif()

if (${b2} STREQUAL "b2-NOTFOUND")
    boost_configure()
endif()

boost_compile()

message("Setting up ext-boost environment variables")
set(BOOST_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/external/ext-boost")
set(BOOST_INCLUDEDIR "${BOOST_ROOT}/boost")
set(BOOST_LIBRARYDIR "${BOOST_INSTALLDIR}/lib")
set(Boost_NO_BOOST_CMAKE ON)
set (Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS thread system regex REQUIRED)
include_directories(${Boost_INCLUDE_DIRS}) 

set(OPENSSL_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/OpenSSL")

message("Using Boost_VERSION: ${BOOST_ROOT}")
message("Using Boost_INCLUDE_DIRS: ${BOOST_INCLUDEDIR}")
message("Using Boost_LIBRARY_DIRS: ${BOOST_LIBRARYDIR}")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 20) 

add_subdirectory(external)
add_subdirectory(src/NumgleCpp)