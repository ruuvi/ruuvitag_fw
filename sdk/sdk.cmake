#
# DA1458x SDK Build rules
# This should be splitted in to SDK source tree
# but we don't want to touch on that.
#

# Lot's of unused params on SDK, don't show warnings
add_definitions(-Wno-unused-parameter)

# enable debug symbols
add_definitions(-g)

set(SDK_ROM_MAPFILE ${SDK_BASEDIR}/misc/rom_symdef.txt)

# create a clean, sorted list of known symbols, used by ROM code
# TODO: one liner this...
file (GENERATE OUTPUT genromsymbols.sh
        CONTENT
        "sed -n -e 's/  */ /g p' ${SDK_ROM_MAPFILE} | sed -e '/^[;#]/d' | sort | dos2unix >rom.symdef
        gawk '{printf \"%s = %s ;\\n\", $3, $1}' rom.symdef > rom.symbols"
        )

add_custom_command(OUTPUT rom.symbols rom.symdef
        COMMAND sh ${CMAKE_BINARY_DIR}/genromsymbols.sh
        DEPENDS ${SDK_ROM_MAPFILE}
        COMMENT "Creating ROM symbols"
        )

add_custom_target(rom_symbols ALL
        COMMAND
        DEPENDS rom.symdef
        )

set(SDK_CORE_INCLUDE_DIRS
	${SDK_SOURCE_DIR}/dialog/include
	${SDK_SOURCE_DIR}/plf/refip/src/arch
	${SDK_SOURCE_DIR}/plf/refip/src/arch/compiler/rvds
	${SDK_SOURCE_DIR}/plf/refip/src/arch/ll/rvds
	${SDK_SOURCE_DIR}/plf/refip/src/arch/boot/rvds
	${SDK_SOURCE_DIR}/plf/refip/src/driver/reg
	${SDK_SOURCE_DIR}/plf/refip/src/driver/syscntl
	${SDK_SOURCE_DIR}/plf/refip/src/driver/emi
	${SDK_SOURCE_DIR}/plf/refip/src/driver/adc
	${SDK_SOURCE_DIR}/plf/refip/src/driver/intc
	${SDK_SOURCE_DIR}/plf/refip/src/driver/timer
	${SDK_SOURCE_DIR}/plf/refip/src/driver/i2c_eeprom
	${SDK_SOURCE_DIR}/plf/refip/src/driver/uart
	${SDK_SOURCE_DIR}/plf/refip/src/driver/gpio
	${SDK_SOURCE_DIR}/plf/refip/src/driver/battery
	${SDK_SOURCE_DIR}/plf/refip/src/driver/spi
	${SDK_SOURCE_DIR}/plf/refip/src/driver/spi_flash
	${SDK_SOURCE_DIR}/plf/refip/src/driver/led
	${SDK_SOURCE_DIR}/plf/refip/src/driver/trng
	${SDK_SOURCE_DIR}/plf/refip/src/driver/wkupct_quadec
	${SDK_SOURCE_DIR}/modules/app/api
	${SDK_SOURCE_DIR}/modules/rwip/api
	${SDK_SOURCE_DIR}/modules/common/api
	${SDK_SOURCE_DIR}/modules/ke/api
	${SDK_SOURCE_DIR}/modules/ke/src
	${SDK_SOURCE_DIR}/modules/dbg/api
	${SDK_SOURCE_DIR}/modules/rf/api
	${SDK_SOURCE_DIR}/modules/gtl/src
	${SDK_SOURCE_DIR}/modules/gtl/api
	${SDK_SOURCE_DIR}/modules/nvds/api
	${SDK_SOURCE_DIR}/ip/ble/ll/src/rwble
	${SDK_SOURCE_DIR}/ip/ble/ll/src/hcic
	${SDK_SOURCE_DIR}/ip/ble/ll/src/controller/em
	${SDK_SOURCE_DIR}/ip/ble/ll/src/controller/lld
	${SDK_SOURCE_DIR}/ip/ble/ll/src/controller/llm
	${SDK_SOURCE_DIR}/ip/ble/ll/src/controller/llc
	${SDK_SOURCE_DIR}/ip/ble/hl/src/rwble_hl
	${SDK_SOURCE_DIR}/ip/ble/hl/src/host/att
	${SDK_SOURCE_DIR}/ip/ble/hl/src/host/att/attc
	${SDK_SOURCE_DIR}/ip/ble/hl/src/host/att/atts
	${SDK_SOURCE_DIR}/ip/ble/hl/src/host/att/attm
	${SDK_SOURCE_DIR}/ip/ble/hl/src/host/smp
	${SDK_SOURCE_DIR}/ip/ble/hl/src/host/smp/smpm
	${SDK_SOURCE_DIR}/ip/ble/hl/src/host/smp/smpc
	${SDK_SOURCE_DIR}/ip/ble/hl/src/host/gap
	${SDK_SOURCE_DIR}/ip/ble/hl/src/host/gap/gapm
	${SDK_SOURCE_DIR}/ip/ble/hl/src/host/gap/gapc
	${SDK_SOURCE_DIR}/ip/ble/hl/src/host/gam
	${SDK_SOURCE_DIR}/ip/ble/hl/src/host/gatt
	${SDK_SOURCE_DIR}/ip/ble/hl/src/host/gatt/gattc
	${SDK_SOURCE_DIR}/ip/ble/hl/src/host/gatt/gattm
	${SDK_SOURCE_DIR}/ip/ble/hl/src/host/l2c/l2cc
	${SDK_SOURCE_DIR}/ip/ble/hl/src/host/l2c/l2cm
	${SDK_SOURCE_DIR}/ip/ble/hl/src/profiles/spota/spotar
	${SDK_SOURCE_DIR}/ip/ble/hl/src/profiles/bas/bass
	${SDK_SOURCE_DIR}/ip/ble/hl/src/profiles/dis/diss
	${SDK_SOURCE_DIR}/ip/ble/hl/src/profiles/device_config
	${SDK_SOURCE_DIR}/ip/ble/hl/src/profiles/prox/proxr
	${SDK_SOURCE_DIR}/ip/ble/hl/src/profiles/find/findl
	${SDK_SOURCE_DIR}/ip/ble/hl/src/profiles
)

set(SDK_APP_INCLUDE_DIRS
	${SDK_SOURCE_DIR}/modules/app/src/app_project/spotar
	${SDK_SOURCE_DIR}/modules/app/src/app_profiles/bass
	${SDK_SOURCE_DIR}/modules/app/src/app_profiles/device_config
	${SDK_SOURCE_DIR}/modules/app/src/app_profiles/diss
	${SDK_SOURCE_DIR}/modules/app/src/app_profiles/prox_reporter
	${SDK_SOURCE_DIR}/modules/app/src/app_profiles/spotar
	${SDK_SOURCE_DIR}/modules/app/src/app_utils/app_config_storage
)

include_directories(${SDK_CORE_INCLUDE_DIRS} ${SDK_APP_INCLUDE_DIRS})


add_library(sdk STATIC
	EXCLUDE_FROM_ALL
	${SDK_SOURCE_DIR}/plf/refip/src/arch/main/ble/arch_main.c
	${SDK_SOURCE_DIR}/plf/refip/src/arch/main/ble/arch_patch.c
	${SDK_SOURCE_DIR}/plf/refip/src/arch/main/ble/arch_sleep.c
	${SDK_SOURCE_DIR}/plf/refip/src/arch/main/ble/arch_system.c
	${SDK_SOURCE_DIR}/plf/refip/src/arch/main/ble/hardfault_handler.c
	${SDK_SOURCE_DIR}/plf/refip/src/arch/main/ble/jump_table.c
	${SDK_SOURCE_DIR}/plf/refip/src/arch/main/ble/nmi_handler.c
	${SDK_SOURCE_DIR}/plf/refip/src/arch/boot/rvds/system_ARMCM0.c
	${SDK_SOURCE_DIR}/plf/refip/src/driver/accel/lis3dh_driver.c
	${SDK_SOURCE_DIR}/plf/refip/src/driver/adc/adc.c
	${SDK_SOURCE_DIR}/plf/refip/src/driver/battery/battery.c
	${SDK_SOURCE_DIR}/plf/refip/src/driver/gpio/gpio.c
	${SDK_SOURCE_DIR}/plf/refip/src/driver/pwm/pwm.c
	${SDK_SOURCE_DIR}/plf/refip/src/driver/rc5/rc5.c
	${SDK_SOURCE_DIR}/plf/refip/src/driver/spi/spi_3wire.c
	${SDK_SOURCE_DIR}/plf/refip/src/driver/spi/spi.c
	${SDK_SOURCE_DIR}/plf/refip/src/driver/spi_flash/spi_flash.c
	${SDK_SOURCE_DIR}/plf/refip/src/driver/syscntl/syscntl.c
	${SDK_SOURCE_DIR}/plf/refip/src/driver/timer/systick.c
	${SDK_SOURCE_DIR}/plf/refip/src/driver/trng/trng.c
	${SDK_SOURCE_DIR}/plf/refip/src/driver/uart/uart_ext_wkup.c
	${SDK_SOURCE_DIR}/plf/refip/src/driver/uart/uart_init.c
	${SDK_SOURCE_DIR}/plf/refip/src/driver/uart/uart2.c
	${SDK_SOURCE_DIR}/plf/refip/src/driver/wkupct_quadec/wkupct_quadec.c
	${SDK_SOURCE_DIR}/ip/ble/ll/src/rwble/rwble.c
	${SDK_SOURCE_DIR}/ip/ble/hl/src/host/gap/gapm/gapm.c
	${SDK_SOURCE_DIR}/ip/ble/hl/src/profiles/accel/accel.c
	${SDK_SOURCE_DIR}/ip/ble/hl/src/profiles/accel/accel_task.c
	${SDK_SOURCE_DIR}/ip/ble/hl/src/profiles/bas/basc/basc.c
	${SDK_SOURCE_DIR}/ip/ble/hl/src/profiles/bas/basc/basc_task.c
	${SDK_SOURCE_DIR}/ip/ble/hl/src/profiles/bas/bass/bass.c
	${SDK_SOURCE_DIR}/ip/ble/hl/src/profiles/bas/bass/bass_task.c
	${SDK_SOURCE_DIR}/ip/ble/hl/src/profiles/blp/blpc/blpc.c
	${SDK_SOURCE_DIR}/ip/ble/hl/src/profiles/blp/blpc/blpc_task.c
	${SDK_SOURCE_DIR}/ip/ble/hl/src/profiles/blp/blps/blps.c
	${SDK_SOURCE_DIR}/ip/ble/hl/src/profiles/blp/blps/blps_task.c
	${SDK_SOURCE_DIR}/ip/ble/hl/src/profiles/dis/disc/disc.c
	${SDK_SOURCE_DIR}/ip/ble/hl/src/profiles/dis/disc/disc_task.c
	${SDK_SOURCE_DIR}/ip/ble/hl/src/profiles/dis/diss/diss.c
	${SDK_SOURCE_DIR}/ip/ble/hl/src/profiles/dis/diss/diss_task.c
	${SDK_SOURCE_DIR}/ip/ble/hl/src/profiles/find/findt/findt.c
	${SDK_SOURCE_DIR}/ip/ble/hl/src/profiles/find/findt/findt_task.c
	${SDK_SOURCE_DIR}/ip/ble/hl/src/profiles/hogp/hogpbh/hogpbh.c
	${SDK_SOURCE_DIR}/ip/ble/hl/src/profiles/hogp/hogpbh/hogpbh_task.c
	${SDK_SOURCE_DIR}/ip/ble/hl/src/profiles/hogp/hogpd/hogpd.c
	${SDK_SOURCE_DIR}/ip/ble/hl/src/profiles/hogp/hogpd/hogpd_task.c
	${SDK_SOURCE_DIR}/ip/ble/hl/src/profiles/hogp/hogprh/hogprh.c
	${SDK_SOURCE_DIR}/ip/ble/hl/src/profiles/hogp/hogprh/hogprh_task.c
	${SDK_SOURCE_DIR}/ip/ble/hl/src/profiles/hrp/hrpc/hrpc.c
	${SDK_SOURCE_DIR}/ip/ble/hl/src/profiles/hrp/hrpc/hrpc_task.c
	${SDK_SOURCE_DIR}/ip/ble/hl/src/profiles/hrp/hrps/hrps.c
	${SDK_SOURCE_DIR}/ip/ble/hl/src/profiles/hrp/hrps/hrps_task.c
	${SDK_SOURCE_DIR}/ip/ble/hl/src/profiles/prf_utils.c
	${SDK_SOURCE_DIR}/ip/ble/hl/src/profiles/prf_utils_128.c
	${SDK_SOURCE_DIR}/ip/ble/hl/src/profiles/device_config/device_config.c
	${SDK_SOURCE_DIR}/ip/ble/hl/src/profiles/device_config/device_config_task.c
	${SDK_SOURCE_DIR}/ip/ble/hl/src/profiles/prox/proxm/proxm.c
	${SDK_SOURCE_DIR}/ip/ble/hl/src/profiles/prox/proxm/proxm_task.c
	${SDK_SOURCE_DIR}/ip/ble/hl/src/profiles/prox/proxr/proxr.c
	${SDK_SOURCE_DIR}/ip/ble/hl/src/profiles/prox/proxr/proxr_task.c
	${SDK_SOURCE_DIR}/ip/ble/hl/src/profiles/spota/spotar/spotar.c
	${SDK_SOURCE_DIR}/ip/ble/hl/src/profiles/spota/spotar/spotar_task.c
	${SDK_SOURCE_DIR}/modules/nvds/src/nvds.c
	${SDK_SOURCE_DIR}/modules/rf/src/rf_580.c
	${SDK_SOURCE_DIR}/modules/rwip/src/rwip.c
	# TODO: Needs defines from app...
	${SDK_SOURCE_DIR}/plf/refip/src/driver/i2c_eeprom/i2c_eeprom.c
	#${SDK_SOURCE_DIR}/plf/refip/src/driver/uart/uart.c
	${SDK_SOURCE_DIR}/modules/app/src/app.c
	${SDK_SOURCE_DIR}/modules/app/src/app_task.c
	${SDK_SOURCE_DIR}/modules/app/src/app_sec.c
	${SDK_SOURCE_DIR}/modules/app/src/app_sec_task.c
	# extension.
	#${SDK_SOURCE_DIR}/modules/app/src/app_utils/app_config_storage/app_config_storage.c
)

add_dependencies(sdk rom_symbols)

#TODO: handle 14581
set(SDK_PATCH_OBJECTS
	${SDK_BASEDIR}/patch_code/DA14580/obj/atts_task.obj
	${SDK_BASEDIR}/patch_code/DA14580/obj/ch_map.obj
	${SDK_BASEDIR}/patch_code/DA14580/obj/gapm_util.obj
	${SDK_BASEDIR}/patch_code/DA14580/obj/ke_mem.obj
	${SDK_BASEDIR}/patch_code/DA14580/obj/ke_task.obj
	${SDK_BASEDIR}/patch_code/DA14580/obj/llc.obj
	${SDK_BASEDIR}/patch_code/DA14580/obj/smpc_task.obj
	${SDK_BASEDIR}/patch_code/DA14580/obj/smpc.obj
)

######

# generate final linker script
# TODO: include dirs from build...
# TODO: handle app specific linker script as ${LINK_SCRIPT_SOURCE}
# is this fully portable?
add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/${LINK_SCRIPT}
        COMMAND ${CMAKE_C_COMPILER} -I ${CMAKE_SOURCE_DIR}/app/${APP_NAME} -E -P ${CMAKE_SOURCE_DIR}/sdk/${LINK_SCRIPT}.S -o ${CMAKE_BINARY_DIR}/${LINK_SCRIPT}
        DEPENDS ${CMAKE_SOURCE_DIR}/sdk/${LINK_SCRIPT}.S
        COMMENT "Generating final Linker script file"
        VERBATIM
        )

add_custom_target(link_script ALL
        COMMAND
        DEPENDS ${CMAKE_BINARY_DIR}/${LINK_SCRIPT}
        )


#
# SDK Helper functions
#

function(ENABLE_PROFILE prof)
	list(APPEND APP_SRCS ${SDK_APP_PROFILES}/${prof}/app_${prof}.c)
	list(APPEND APP_SRCS ${SDK_APP_PROFILES}/${prof}/app_${prof}_task.c)
	set(APP_SRCS ${APP_SRCS} PARENT_SCOPE)
endfunction()

