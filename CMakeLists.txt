#
# CMAKE BUILDSYSTEM FOR RUUVITAG.
#
# Copyright (c) 2015, Offcode Ltd. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#    * Redistributions of source code must retain the above copyright
#      notice,   this list of conditions and the following disclaimer.
#    * Redistributions in  binary form must  reproduce the above copyright
#      notice, this list of conditions and the following disclaimer in the
#      documentation and/or other materials provided with the distribution.
#    * Neither the name of the RuuviTag nor the
#      names of its contributors may be used to endorse or promote products
#      derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND  ANY  EXPRESS  OR  IMPLIED WARRANTIES,  INCLUDING,  BUT NOT LIMITED TO,
# THE  IMPLIED  WARRANTIES  OF MERCHANTABILITY  AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL COPYRIGHT OWNER OR CONTRIBUTORS
# BE LIABLE FOR ANY DIRECT,  INDIRECT,  INCIDENTAL,  SPECIAL,  EXEMPLARY, OR
# CONSEQUENTIAL  DAMAGES  (INCLUDING,  BUT  NOT  LIMITED  TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES;  LOSS OF USE, DATA, OR PROFITS;  OR BUSINESS
# INTERRUPTION)  HOWEVER CAUSED AND  ON ANY THEORY OF LIABILITY,  WHETHER IN
# CONTRACT,  STRICT LIABILITY,  OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#

cmake_minimum_required(VERSION 3.2)
cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0056 NEW)

set (CMAKE_MODULE_PATH
        ${CMAKE_SOURCE_DIR}/cmake/modules
        ${CMAKE_SOURCE_DIR}/cmake)

# include custom tools and macros
include (tools)

# find some common packages.
find_package(Git)

if (${CMAKE_CURRENT_BINARY_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
        message (FATAL_ERROR "Source and build directories are same. Not supported!, please clean manually")
endif()

# load config from "active project"
#
if (EXISTS default-config.cmake)
  if (CMAKE_CROSSCOMPILING)
    include (default-config.cmake)
  endif()
else()
endif()

# SDK needs application config headers
if (NOT APP_NAME)
        set(APP_NAME $ENV{APP_NAME})
        if (NOT APP_NAME)
        message(FATAL_ERROR "APP_NAME not defined!!!...")
        endif()
endif()

include (app/${APP_NAME}/config.cmake)

# Detect compilers now...
project (${APP_NAME}
        VERSION ${APP_VERSION}
        LANGUAGES C ASM)

message("------------------------------------------")
message("         APP_NAME: ${APP_NAME}")
message("------------------------------------------")

if (CMAKE_CROSSCOMPILING)
  message("--Cross compiling")

else(CMAKE_CROSSCOMPILING)
        message("--Compiling for host")
        enable_testing()
        set(TESTING_ENABLED YES)
        message("TODO: Host testing...")
        return()
endif(CMAKE_CROSSCOMPILING)

# set lib output path
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set (DOCS_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/docs)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set (EXECUTABLE_OUTPUT_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# all generated includes goes here.
set (INCLUDE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/include)

# Add for all sub targets.
include_directories(${CMAKE_BINARY_DIR})
include_directories(${INCLUDE_OUTPUT_DIRECTORY})

# don't complain about unknown attributes (i.e. zero_init)
add_definitions(-Wno-attributes)

# global configuration
add_definitions(-include da14580_config.h)

# SDK
add_subdirectory(sdk)

include_directories(${SDK_CORE_INCLUDE_DIRS} ${SDK_APP_INCLUDE_DIRS})

# process APP sources...
add_subdirectory(app/${APP_NAME})

if (EXISTS docs)
        add_subdirectory(docs)
endif()
