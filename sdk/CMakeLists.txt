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

# TODO: support multiple SDK versions
# only 3.0.8 supported now.
set (SDK_BASEDIR ${CMAKE_CURRENT_SOURCE_DIR}/DA14580_581_SDK_3.0.8.0/dk_apps)

set (SDK_PATCH_DIR ${SDK_BASEDIR}/patch_code CACHE FILEPATH "SDK patch Directory")
set (SDK_SOURCE_DIR ${SDK_BASEDIR}/src CACHE FILEPATH "SDK Source Directory")

# Currently _not_ possible to automaticlly download SDK...
# TODO: write howto!
if (NOT EXISTS ${SDK_SOURCE_DIR})
        #message("SDK missing, trying to download...")
        message("SDK missing, see README.md for info")
        message (FATAL_ERROR "DOWLOAD SDK from http://support.dialog-semiconductor.com/product/da14580")
        # SDK 3.0.8
        #file (DOWNLOAD "http://support.dialog-semiconductor.com/download/file/fid/779"
        #${CMAKE_BINARY_DIR}/sdk.zip
        #SHOW_PROGRESS
        #LOG _downlog
        #)
        #message("Download LOG:" ${_downlog})
        # TODO: install...
        return()
endif()

function(patch_sdk patch)
	execute_process(COMMAND ${PATCH_EXECUTABLE} -p2 --directory=${SDK_BASEDIR}/src
			INPUT_FILE ${CMAKE_SOURCE_DIR}/sdk/patch/${patch}.patch
			RESULT_VARIABLE _res
			OUTPUT_VARIABLE _output
			ERROR_VARIABLE  _err)
	message("  > " ${_output})
endfunction()

if (NOT EXISTS ${SDK_BASEDIR}/.patched)
	find_program(PATCH_EXECUTABLE patch)

	if (NOT PATCH_EXECUTABLE)
		message(WARNING "patch executable not found!")
		return()
	endif()

	message("Apply ruuviTag patches to SDK source tree...")
	patch_sdk(compiler)
	patch_sdk(arch)
	patch_sdk(rwble)
	file(WRITE ${SDK_BASEDIR}/.patched "done.")
endif()

# TODO: revert patches -R


### check some files we can't distribute yet

if (NOT EXISTS ${CMAKE_SOURCE_DIR}/app/${APP_NAME}/da14580_stack_config.h)
	file (INSTALL ${SDK_BASEDIR}/keil_projects/template/template/da14580_stack_config.h
		DESTINATION ${CMAKE_SOURCE_DIR}/app/${APP_NAME}
	)
endif()

if (NOT EXISTS ${CMAKE_SOURCE_DIR}/app/${APP_NAME}/da14580_scatter_config.h)
	file (INSTALL ${SDK_BASEDIR}/keil_projects/template/template/da14580_scatter_config.h
		DESTINATION ${CMAKE_SOURCE_DIR}/app/${APP_NAME}
	)
endif()

###

set (LINK_SCRIPT 580.lds CACHE STRING "Target Linker script")
set (SDK_APP_PROFILES ${SDK_SOURCE_DIR}/modules/app/src/app_profiles)

include(sdk.cmake)

# export to globals
set(SDK_APP_PROFILES  ${SDK_APP_PROFILES}  CACHE STRING "Application Profiles dir")
set(SDK_PATCH_OBJECTS ${SDK_PATCH_OBJECTS} PARENT_SCOPE)

# Include directories needed by application
set(SDK_CORE_INCLUDE_DIRS ${SDK_CORE_INCLUDE_DIRS} PARENT_SCOPE)
set(SDK_APP_INCLUDE_DIRS  ${SDK_APP_INCLUDE_DIRS}  PARENT_SCOPE)
