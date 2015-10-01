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

# stack size
set (STACK_SIZE 0x0600)

# heap size
set (HEAP_SIZE 0x0010)

set(STARTUP "startup_ARMCM0.S")

# startup configuration
add_definitions(-D__STACK_SIZE=${STACK_SIZE} -D__HEAP_SIZE=${HEAP_SIZE})
#add_definitions(-D__STARTUP_COPY_MULTIPLE)
add_definitions(-D__STARTUP_CLEAR_BSS_MULTIPLE)

# don't complain about unknown attributes (i.e. zero_init)
add_definitions(-Wno-attributes)

add_definitions(-flto)
# create debug symbols
add_definitions(-g)

set(APP_SRCS
        ${STARTUP}
        app_demo_proj.c
        system/periph_setup.c
        system/app_sleep.h
        #system/i2c_bus.c
        da14580_config.h
        #adxl363_driver.c
        #ms5637_driver.c
)

#ENABLE_PROFILE(bass)
#ENABLE_PROFILE(sps/sps_server)
#list(APPEND APP_SRCS ${SDK_APP_PROFILES}/sps/sps_server/sps_server.c)

add_executable(${APP_NAME} ${APP_SRCS} ${SDK_PATCH_OBJECTS})
create_bin_from(${APP_NAME})
create_hex_from(${APP_NAME})
target_link_libraries(${APP_NAME} sdk)
target_link_libraries(${APP_NAME} "-Wl,-T${CMAKE_BINARY_DIR}/${LINK_SCRIPT}")
target_link_libraries(${APP_NAME} "-Wl,-Map=${CMAKE_BINARY_DIR}/${APP_NAME}.map")
target_link_libraries(${APP_NAME} "-Wl,--gc-sections")
# FIXME:
target_link_libraries(${APP_NAME} "-L ${CMAKE_BINARY_DIR}/sdk")
# SDK Wraps... test
#target_link_libraries(${APP_NAME} "-Wl,--wrap=BLE_RX_Handler")
#target_link_libraries(${APP_NAME} "-Wl,--wrap=BLE_EVENT_Handler")

target_link_libraries(${APP_NAME} "-Wl,--wrap=BLE_CSCNT_Handler")

#target_link_libraries(${APP_NAME} "-Wl,--wrap=BLE_FINETGTIM_Handler")
#target_link_libraries(${APP_NAME} "-Wl,--wrap=BLE_RF_DIAG_Handler")
target_link_libraries(${APP_NAME} "-Wl,--wrap=get_rc16m_count_func")

# REVISIT:
# "uses 2-byte wchar_t yet the output is to use 4-byte wchar_t;
# use of wchar_t values across objects may fail"
# revisit if things don't work as expected with wchar_t strings.
target_link_libraries(${APP_NAME} "-Wl,--no-wchar-size-warning")


