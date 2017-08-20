PROJECT_NAME     := test_drivers_ruuvitag_b5_s132
TARGETS          := test_drivers
OUTPUT_DIRECTORY := _build

SDK_ROOT := ../../../../../nRF5_SDK_12.3.0_d7731ad
IOTA_ROOT := ../../../../../iota.lib.c
PROJ_DIR := ../../..

$(OUTPUT_DIRECTORY)/test_drivers.out: \
  LINKER_SCRIPT  := test_drivers.ld
# Source files common to all targets
SRC_FILES += \
  $(SDK_ROOT)/components/ble/common/ble_advdata.c \
  $(SDK_ROOT)/components/ble/common/ble_conn_params.c \
  $(SDK_ROOT)/components/ble/common/ble_conn_state.c \
  $(SDK_ROOT)/components/ble/common/ble_srv_common.c \
  $(SDK_ROOT)/components/ble/nrf_ble_qwr/nrf_ble_qwr.c \
  $(SDK_ROOT)/components/ble/peer_manager/gatt_cache_manager.c \
  $(SDK_ROOT)/components/ble/peer_manager/gatts_cache_manager.c \
  $(SDK_ROOT)/components/ble/peer_manager/id_manager.c \
  $(SDK_ROOT)/components/ble/peer_manager/peer_data.c \
  $(SDK_ROOT)/components/ble/peer_manager/peer_data_storage.c \
  $(SDK_ROOT)/components/ble/peer_manager/peer_database.c \
  $(SDK_ROOT)/components/ble/peer_manager/peer_manager.c \
  $(SDK_ROOT)/components/ble/peer_manager/peer_id.c \
  $(SDK_ROOT)/components/ble/peer_manager/pm_buffer.c \
  $(SDK_ROOT)/components/ble/peer_manager/pm_mutex.c \
  $(SDK_ROOT)/components/ble/peer_manager/security_dispatcher.c \
  $(SDK_ROOT)/components/ble/peer_manager/security_manager.c \
  $(SDK_ROOT)/components/ble/ble_services/ble_nus/ble_nus.c \
  $(SDK_ROOT)/components/drivers_nrf/clock/nrf_drv_clock.c \
  $(SDK_ROOT)/components/drivers_nrf/clock/nrf_drv_clock.c \
  $(SDK_ROOT)/components/drivers_nrf/common/nrf_drv_common.c \
  $(SDK_ROOT)/components/drivers_nrf/gpiote/nrf_drv_gpiote.c \
  $(SDK_ROOT)/components/drivers_nrf/hal/nrf_saadc.c \
  $(SDK_ROOT)/components/drivers_nrf/rng/nrf_drv_rng.c \
  $(SDK_ROOT)/components/drivers_nrf/rtc/nrf_drv_rtc.c \
  $(SDK_ROOT)/components/drivers_nrf/saadc/nrf_drv_saadc.c \
  $(SDK_ROOT)/components/drivers_nrf/spi_master/nrf_drv_spi.c \
  $(SDK_ROOT)/components/drivers_nrf/timer/nrf_drv_timer.c \
  $(SDK_ROOT)/components/drivers_nrf/uart/nrf_drv_uart.c \
  $(SDK_ROOT)/components/ble/ble_advertising/ble_advertising.c \
  $(SDK_ROOT)/components/libraries/fifo/app_fifo.c \
  $(SDK_ROOT)/components/libraries/fds/fds.c \
  $(SDK_ROOT)/components/libraries/fstorage/fstorage.c \
  $(SDK_ROOT)/components/libraries/hardfault/hardfault_implementation.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_serial.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_frontend.c \
  $(SDK_ROOT)/components/libraries/queue/nrf_queue.c \
  $(SDK_ROOT)/components/libraries/scheduler/app_scheduler.c \
  $(SDK_ROOT)/components/libraries/timer/app_timer_appsh.c \
  $(SDK_ROOT)/components/libraries/timer/app_timer.c \
  $(SDK_ROOT)/components/libraries/util/app_error_weak.c \
  $(SDK_ROOT)/components/libraries/util/app_error.c \
  $(SDK_ROOT)/components/libraries/util/app_util_platform.c \
  $(SDK_ROOT)/components/libraries/util/nrf_assert.c \
  $(SDK_ROOT)/components/libraries/util/sdk_errors.c \
  $(SDK_ROOT)/components/libraries/util/sdk_mapped_flags.c \
  $(SDK_ROOT)/components/softdevice/common/softdevice_handler/softdevice_handler.c \
  $(SDK_ROOT)/components/toolchain/gcc/gcc_startup_nrf52.S \
  $(SDK_ROOT)/components/toolchain/system_nrf52.c \
  $(SDK_ROOT)/external/segger_rtt/RTT_Syscalls_GCC.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT_printf.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT.c \
  $(IOTA_ROOT)/lib/utils/asciiToTrytes.c \
  $(PROJ_DIR)/../../bsp/bsp.c \
  $(PROJ_DIR)/../../bsp/bsp_btn_ble.c \
  $(PROJ_DIR)/../../bsp/bsp_nfc.c \
  $(PROJ_DIR)/../../bsp/boards.c \
  $(PROJ_DIR)/../../drivers/battery/battery.c \
  $(PROJ_DIR)/../../drivers/bluetooth/ble_bulk_transfer.c \
  $(PROJ_DIR)/../../drivers/bluetooth/ble_event_handlers.c \
  $(PROJ_DIR)/../../drivers/bluetooth/bluetooth_core.c \
  $(PROJ_DIR)/../../drivers/bme280/bme280.c \
  $(PROJ_DIR)/../../drivers/init/init.c \
  $(PROJ_DIR)/../../drivers/LIS2DH12/LIS2DH12.c \
  $(PROJ_DIR)/../../drivers/rng/rng.c \
  $(PROJ_DIR)/../../drivers/rtc/rtc.c \
  $(PROJ_DIR)/../../drivers/spi/spi.c \
  $(PROJ_DIR)/../../libraries/acceleration/acceleration.c \
  $(PROJ_DIR)/../../libraries/rust_allocator/rust_allocator.c \
  $(PROJ_DIR)/../../sdk_overrides/app_button.c \
  $(PROJ_DIR)/ble_services/application_ble_event_handlers.c \
  $(PROJ_DIR)/ble_services/application_service_if.c \
  $(PROJ_DIR)/tests/test_environmental.c \
  $(PROJ_DIR)/tests/test_mam.c \
  $(PROJ_DIR)/tests/test_rng.c \
  $(PROJ_DIR)/tests/test_rtc.c \
  $(PROJ_DIR)/main.c \

# Include folders common to all targets
INC_FOLDERS += \
  $(SDK_ROOT)/components \
  $(SDK_ROOT)/components/ble/ble_advertising \
  $(SDK_ROOT)/components/ble/ble_dtm \
  $(SDK_ROOT)/components/ble/ble_racp \
  $(SDK_ROOT)/components/ble/ble_services/ble_ancs_c \
  $(SDK_ROOT)/components/ble/ble_services/ble_ans_c \
  $(SDK_ROOT)/components/ble/ble_services/ble_bas \
  $(SDK_ROOT)/components/ble/ble_services/ble_bas_c \
  $(SDK_ROOT)/components/ble/ble_services/ble_cscs \
  $(SDK_ROOT)/components/ble/ble_services/ble_cts_c \
  $(SDK_ROOT)/components/ble/ble_services/ble_dfu \
  $(SDK_ROOT)/components/ble/ble_services/ble_dis \
  $(SDK_ROOT)/components/ble/ble_services/ble_gls \
  $(SDK_ROOT)/components/ble/ble_services/ble_hids \
  $(SDK_ROOT)/components/ble/ble_services/ble_hrs \
  $(SDK_ROOT)/components/ble/ble_services/ble_hrs_c \
  $(SDK_ROOT)/components/ble/ble_services/ble_hts \
  $(SDK_ROOT)/components/ble/ble_services/ble_ias \
  $(SDK_ROOT)/components/ble/ble_services/ble_ias_c \
  $(SDK_ROOT)/components/ble/ble_services/ble_lbs \
  $(SDK_ROOT)/components/ble/ble_services/ble_lbs_c \
  $(SDK_ROOT)/components/ble/ble_services/ble_lls \
  $(SDK_ROOT)/components/ble/ble_services/ble_nus \
  $(SDK_ROOT)/components/ble/ble_services/ble_nus_c \
  $(SDK_ROOT)/components/ble/ble_services/ble_rscs \
  $(SDK_ROOT)/components/ble/ble_services/ble_rscs_c \
  $(SDK_ROOT)/components/ble/ble_services/ble_tps \
  $(SDK_ROOT)/components/ble/common \
  $(SDK_ROOT)/components/ble/nrf_ble_qwr \
  $(SDK_ROOT)/components/ble/peer_manager \
  $(SDK_ROOT)/components/device \
  $(SDK_ROOT)/components/drivers_nrf/adc \
  $(SDK_ROOT)/components/drivers_nrf/clock \
  $(SDK_ROOT)/components/drivers_nrf/clock \
  $(SDK_ROOT)/components/drivers_nrf/common \
  $(SDK_ROOT)/components/drivers_nrf/comp \
  $(SDK_ROOT)/components/drivers_nrf/delay \
  $(SDK_ROOT)/components/drivers_nrf/delay \
  $(SDK_ROOT)/components/drivers_nrf/gpiote \
  $(SDK_ROOT)/components/drivers_nrf/hal \
  $(SDK_ROOT)/components/drivers_nrf/i2s \
  $(SDK_ROOT)/components/drivers_nrf/lpcomp \
  $(SDK_ROOT)/components/drivers_nrf/pdm \
  $(SDK_ROOT)/components/drivers_nrf/ppi \
  $(SDK_ROOT)/components/drivers_nrf/pwm \
  $(SDK_ROOT)/components/drivers_nrf/qdec \
  $(SDK_ROOT)/components/drivers_nrf/rng \
  $(SDK_ROOT)/components/drivers_nrf/rtc \
  $(SDK_ROOT)/components/drivers_nrf/saadc \
  $(SDK_ROOT)/components/drivers_nrf/spi_master \
  $(SDK_ROOT)/components/drivers_nrf/spi_slave \
  $(SDK_ROOT)/components/drivers_nrf/swi \
  $(SDK_ROOT)/components/drivers_nrf/timer \
  $(SDK_ROOT)/components/drivers_nrf/twi_master \
  $(SDK_ROOT)/components/drivers_nrf/twis_slave \
  $(SDK_ROOT)/components/drivers_nrf/uart \
  $(SDK_ROOT)/components/drivers_nrf/wdt \
  $(SDK_ROOT)/components/libraries/button \
  $(SDK_ROOT)/components/libraries/crc16 \
  $(SDK_ROOT)/components/libraries/crc32 \
  $(SDK_ROOT)/components/libraries/csense \
  $(SDK_ROOT)/components/libraries/csense_drv \
  $(SDK_ROOT)/components/libraries/experimental_section_vars \
  $(SDK_ROOT)/components/libraries/fds \
  $(SDK_ROOT)/components/libraries/fifo \
  $(SDK_ROOT)/components/libraries/fstorage \
  $(SDK_ROOT)/components/libraries/hardfault \
  $(SDK_ROOT)/components/libraries/hci \
  $(SDK_ROOT)/components/libraries/led_softblink \
  $(SDK_ROOT)/components/libraries/log \
  $(SDK_ROOT)/components/libraries/log/src \
  $(SDK_ROOT)/components/libraries/low_power_pwm \
  $(SDK_ROOT)/components/libraries/mailbox \
  $(SDK_ROOT)/components/libraries/mem_manager \
  $(SDK_ROOT)/components/libraries/pwm \
  $(SDK_ROOT)/components/libraries/queue \
  $(SDK_ROOT)/components/libraries/scheduler \
  $(SDK_ROOT)/components/libraries/scheduler \
  $(SDK_ROOT)/components/libraries/slip \
  $(SDK_ROOT)/components/libraries/timer \
  $(SDK_ROOT)/components/libraries/timer \
  $(SDK_ROOT)/components/libraries/twi \
  $(SDK_ROOT)/components/libraries/uart \
  $(SDK_ROOT)/components/libraries/util \
  $(SDK_ROOT)/components/softdevice/common/softdevice_handler \
  $(SDK_ROOT)/components/softdevice/s132/headers \
  $(SDK_ROOT)/components/softdevice/s132/headers/nrf52 \
  $(SDK_ROOT)/components/toolchain \
  $(SDK_ROOT)/components/toolchain/cmsis/include \
  $(SDK_ROOT)/components/toolchain/gcc \
  $(SDK_ROOT)/external/segger_rtt \
  $(IOTA_ROOT)/external/ccurl/src/lib/ \
  $(IOTA_ROOT)/include \
  $(IOTA_ROOT)/lib/utils \
  $(PROJ_DIR)/ble_services/ \
  $(PROJ_DIR)/tests/ \
  $(PROJ_DIR)/ \
  $(PROJ_DIR)/../../bsp \
  $(PROJ_DIR)/../../drivers/bluetooth \
  $(PROJ_DIR)/../../drivers/battery/ \
  $(PROJ_DIR)/../../drivers/bme280 \
  $(PROJ_DIR)/../../drivers/init \
  $(PROJ_DIR)/../../drivers/LIS2DH12 \
  $(PROJ_DIR)/../../drivers/rng/ \
  $(PROJ_DIR)/../../drivers/rtc/ \
  $(PROJ_DIR)/../../drivers/spi/ \
  $(PROJ_DIR)/../../libraries/acceleration \
  $(PROJ_DIR)/../../libraries/rust_allocator/ \
  $(PROJ_DIR)/../../libraries/ruuvi_sensor_formats/ \
  ../config \

# Libraries common to all targets
LIB_FILES += \
  ../../../libiota_mam_bindings.a
  
# C flags common to all targets
CFLAGS += -DNRF52
CFLAGS += -DBOARD_CUSTOM
CFLAGS += -DBOARD_RUUVITAG_B5
CFLAGS += -DNRF52832
CFLAGS += -DNRF52_PAN_64
CFLAGS += -DNRF52_PAN_12
CFLAGS += -DNRF52_PAN_15
CFLAGS += -DNRF52_PAN_58
CFLAGS += -DNRF52_PAN_55
CFLAGS += -DNRF52_PAN_54
CFLAGS += -DNRF52_PAN_31
CFLAGS += -DNRF52_PAN_30
CFLAGS += -DNRF52_PAN_51
CFLAGS += -DNRF52_PAN_36
CFLAGS += -DNRF52_PAN_53
CFLAGS += -DS132
CFLAGS += -DCONFIG_GPIO_AS_PINRESET
CFLAGS += -DBLE_STACK_SUPPORT_REQD
CFLAGS += -DNRF_SD_BLE_API_VERSION=3
CFLAGS += -DSWI_DISABLE0
CFLAGS += -DNRF52_PAN_20
CFLAGS += -DSOFTDEVICE_PRESENT
CFLAGS += -DNRF52_PAN_62
CFLAGS += -DNRF52_PAN_63
CFLAGS += -mcpu=cortex-m4
CFLAGS += -mthumb -mabi=aapcs
CFLAGS +=  -Wall -Werror -O0 -g3
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# keep every function in separate section, this allows linker to discard unused ones
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin --short-enums 
# generate dependency output file
CFLAGS += -MP -MD

# C++ flags common to all targets
CXXFLAGS += \

# Assembler flags common to all targets
ASMFLAGS += -x assembler-with-cpp
ASMFLAGS += -DNRF52
ASMFLAGS += -DBOARD_CUSTOM
ASMFLAGS += -DBOARD_RUUVITAG_B3
ASMFLAGS += -DNRF52832
ASMFLAGS += -DNRF52_PAN_64
ASMFLAGS += -DNRF52_PAN_12
ASMFLAGS += -DNRF52_PAN_15
ASMFLAGS += -DNRF52_PAN_58
ASMFLAGS += -DNRF52_PAN_55
ASMFLAGS += -DNRF52_PAN_54
ASMFLAGS += -DNRF52_PAN_31
ASMFLAGS += -DNRF52_PAN_30
ASMFLAGS += -DNRF52_PAN_51
ASMFLAGS += -DNRF52_PAN_36
ASMFLAGS += -DNRF52_PAN_53
ASMFLAGS += -DS132
ASMFLAGS += -DCONFIG_GPIO_AS_PINRESET
ASMFLAGS += -DBLE_STACK_SUPPORT_REQD
ASMFLAGS += -DNRF_SD_BLE_API_VERSION=3
ASMFLAGS += -DSWI_DISABLE0
ASMFLAGS += -DNRF52_PAN_20
ASMFLAGS += -DSOFTDEVICE_PRESENT
ASMFLAGS += -DNRF52_PAN_62
ASMFLAGS += -DNRF52_PAN_63

# Linker flags
LDFLAGS += -mthumb -mabi=aapcs -L $(TEMPLATE_PATH) -T$(LINKER_SCRIPT)
LDFLAGS += -mcpu=cortex-m4
LDFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# let linker to dump unused sections
LDFLAGS += -Wl,--gc-sections
# use newlib in nano version
LDFLAGS += --specs=nano.specs -lc -lnosys

.PHONY: $(TARGETS) default all clean help flash  flash_softdevice
# Default target - first one defined
default: test_drivers

# Print all targets that can be built
help:
	@echo following targets are available:
	@echo 	test_drivers
	@echo 	flash_softdevice

TEMPLATE_PATH := $(SDK_ROOT)/components/toolchain/gcc

include $(TEMPLATE_PATH)/Makefile.common
$(foreach target, $(TARGETS), $(call define_target, $(target)))
-include $(foreach target, $(TARGETS), $($(target)_dependencies))

