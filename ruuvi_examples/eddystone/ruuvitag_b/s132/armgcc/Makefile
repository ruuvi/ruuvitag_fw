PROJECT_NAME     := eddystone_ruuvitag-b_s132
TARGETS          := eddystone
OUTPUT_DIRECTORY := _build

SDK_ROOT := ../../../../../nRF5_SDK_12.3.0_d7731ad
PROJ_DIR := ../../..

$(OUTPUT_DIRECTORY)/eddystone.out: \
  LINKER_SCRIPT := eddystone.ld

# Source files common to all targets
SRC_FILES += \
  $(PROJ_DIR)/main.c \
  $(PROJ_DIR)/event_handlers.c \
  $(PROJ_DIR)/ble_services/application_ble_event_handlers.c \
  $(PROJ_DIR)/ble_services/application_service_if.c \
  $(PROJ_DIR)/../../sdk_overrides/es_battery_voltage_saadc.c \
  $(PROJ_DIR)/ble_services/nrf_dfu_flash_buttonless.c \
  $(PROJ_DIR)/../../bsp/bsp.c \
  $(PROJ_DIR)/../../bsp/bsp_btn_ble.c \
  $(PROJ_DIR)/../../bsp/bsp_nfc.c \
  $(PROJ_DIR)/../../bsp/boards.c \
  $(PROJ_DIR)/../../drivers/bluetooth/ble_bulk_transfer.c \
  $(PROJ_DIR)/../../drivers/bluetooth/ble_event_handlers.c \
  $(PROJ_DIR)/../../drivers/bluetooth/bluetooth_core.c \
  $(PROJ_DIR)/../../drivers/bme280/bme280.c \
  $(PROJ_DIR)/../../drivers/bme280/bme280_temperature_handler.c \
  $(PROJ_DIR)/../../drivers/init/init.c \
  $(PROJ_DIR)/../../drivers/lis2dh12/lis2dh12.c \
  $(PROJ_DIR)/../../drivers/lis2dh12/lis2dh12_acceleration_handler.c \
  $(PROJ_DIR)/../../drivers/nrf_nordic_nfc/nfc.c \
  $(PROJ_DIR)/../../drivers/nrf_nordic_nfc/nrf_nfc_handler.c \
  $(PROJ_DIR)/../../drivers/nrf_nordic_pininterrupt/pin_interrupt.c \
  $(PROJ_DIR)/../../drivers/nrf_nordic_watchdog/watchdog.c \
  $(PROJ_DIR)/../../drivers/pwm/pwm.c \
  $(PROJ_DIR)/../../drivers/spi/spi.c \
  $(PROJ_DIR)/../../libraries/ruuvi_sensor_formats/ruuvi_endpoints.c \
  $(PROJ_DIR)/../../libraries/ruuvi_sensor_formats/chain_channels.c \
  $(PROJ_DIR)/../../libraries/data_structures/ringbuffer.c \
  $(PROJ_DIR)/../../libraries/dsp/dsp.c \
  $(PROJ_DIR)/../../libraries/dsp/stdev.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_serial.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_frontend.c \
  $(SDK_ROOT)/external/tiny-AES128/aes.c \
  $(SDK_ROOT)/components/libraries/button/app_button.c \
  $(SDK_ROOT)/components/libraries/util/app_error.c \
  $(SDK_ROOT)/components/libraries/util/app_error_weak.c \
  $(SDK_ROOT)/components/libraries/scheduler/app_scheduler.c \
  $(SDK_ROOT)/components/libraries/timer/app_timer.c \
  $(SDK_ROOT)/components/libraries/timer/app_timer_appsh.c \
  $(SDK_ROOT)/components/libraries/util/app_util_platform.c \
  $(SDK_ROOT)/external/cifra_AES128-EAX/blockwise.c \
  $(SDK_ROOT)/external/cifra_AES128-EAX/cifra_eax_aes.c \
  $(SDK_ROOT)/external/cifra_AES128-EAX/cmac.c \
  $(SDK_ROOT)/components/libraries/crc16/crc16.c \
  $(SDK_ROOT)/external/cifra_AES128-EAX/eax.c \
  $(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu_settings.c \
  $(SDK_ROOT)/components/libraries/crc16/crc16.c \
  $(SDK_ROOT)/components/libraries/crc32/crc32.c \
  $(SDK_ROOT)/components/libraries/eddystone/es_adv.c \
  $(SDK_ROOT)/components/libraries/eddystone/es_adv_frame.c \
  $(SDK_ROOT)/components/libraries/eddystone/es_adv_timing.c \
  $(SDK_ROOT)/components/libraries/eddystone/es_adv_timing_resolver.c \
  $(SDK_ROOT)/components/libraries/eddystone/es_flash.c \
  $(SDK_ROOT)/components/libraries/eddystone/es_gatts.c \
  $(SDK_ROOT)/components/libraries/eddystone/es_gatts_read.c \
  $(SDK_ROOT)/components/libraries/eddystone/es_gatts_write.c \
  $(SDK_ROOT)/components/libraries/eddystone/es_security.c \
  $(SDK_ROOT)/components/libraries/eddystone/es_security_timing.c \
  $(SDK_ROOT)/components/libraries/eddystone/es_slot.c \
  $(SDK_ROOT)/components/libraries/eddystone/es_slot_reg.c \
  $(SDK_ROOT)/components/libraries/eddystone/es_stopwatch.c \
  $(SDK_ROOT)/components/libraries/eddystone/es_tlm.c \
  $(SDK_ROOT)/components/libraries/fds/fds.c \
  $(SDK_ROOT)/components/libraries/fstorage/fstorage.c \
  $(SDK_ROOT)/external/cifra_AES128-EAX/gf128.c \
  $(SDK_ROOT)/components/libraries/hardfault/hardfault_implementation.c \
  $(SDK_ROOT)/external/cifra_AES128-EAX/modes.c \
  $(SDK_ROOT)/components/libraries/util/nrf_assert.c \
  $(SDK_ROOT)/components/libraries/eddystone/nrf_ble_es.c \
  $(SDK_ROOT)/components/libraries/queue/nrf_queue.c \
  $(SDK_ROOT)/components/libraries/util/sdk_errors.c \
  $(SDK_ROOT)/components/libraries/util/sdk_mapped_flags.c \
  $(SDK_ROOT)/components/libraries/pwm/app_pwm.c \
  $(SDK_ROOT)/components/libraries/sensorsim/sensorsim.c \
  $(SDK_ROOT)/external/segger_rtt/RTT_Syscalls_GCC.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT_printf.c \
  $(SDK_ROOT)/components/drivers_nrf/clock/nrf_drv_clock.c \
  $(SDK_ROOT)/components/drivers_nrf/common/nrf_drv_common.c \
  $(SDK_ROOT)/components/drivers_nrf/gpiote/nrf_drv_gpiote.c \
  $(SDK_ROOT)/components/drivers_nrf/saadc/nrf_drv_saadc.c \
  $(SDK_ROOT)/components/drivers_nrf/spi_master/nrf_drv_spi.c \
  $(SDK_ROOT)/components/drivers_nrf/uart/nrf_drv_uart.c \
  $(SDK_ROOT)/components/drivers_nrf/hal/nrf_saadc.c \
  $(SDK_ROOT)/components/drivers_nrf/wdt/nrf_drv_wdt.c \
  $(SDK_ROOT)/components/softdevice/common/softdevice_handler/softdevice_handler.c \
  $(SDK_ROOT)/components/ble/ble_advertising/ble_advertising.c \
  $(SDK_ROOT)/components/ble/ble_services/ble_dfu/ble_dfu.c \
  $(SDK_ROOT)/components/ble/ble_services/ble_dis/ble_dis.c \
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
  $(SDK_ROOT)/components/toolchain/gcc/gcc_startup_nrf52.S \
  $(SDK_ROOT)/components/toolchain/system_nrf52.c \
  $(SDK_ROOT)/components/ble/ble_services/ble_escs/nrf_ble_escs.c \
  $(SDK_ROOT)/components/nfc/t2t_lib/hal_t2t/hal_nfc_t2t.c \
  $(SDK_ROOT)/components/nfc/ndef/generic/message/nfc_ndef_msg.c \
  $(SDK_ROOT)/components/nfc/ndef/generic/record/nfc_ndef_record.c \
  $(SDK_ROOT)/components/nfc/ndef/text/nfc_text_rec.c

# Include folders common to all targets
INC_FOLDERS += \
  $(PROJ_DIR)/ \
  $(PROJ_DIR)/ble_services/ \
  $(PROJ_DIR)/../../bsp \
  $(PROJ_DIR)/../../drivers/battery/ \
  $(PROJ_DIR)/../../drivers/bluetooth/ \
  $(PROJ_DIR)/../../drivers/bme280 \
  $(PROJ_DIR)/../../drivers/lis2dh12 \
  $(PROJ_DIR)/../../drivers/spi/ \
  $(PROJ_DIR)/../../drivers/init/ \
  $(PROJ_DIR)/../../drivers/nrf_nordic_nfc/ \
  $(PROJ_DIR)/../../drivers/nrf_nordic_pininterrupt/ \
  $(PROJ_DIR)/../../drivers/pwm/ \
  $(PROJ_DIR)/../../drivers/nrf_nordic_watchdog/ \
  $(PROJ_DIR)/../../libraries/data_structures/ \
  $(PROJ_DIR)/../../libraries/dsp/ \
  $(PROJ_DIR)/../../libraries/ruuvi_sensor_formats/ \
  $(PROJ_DIR)/ruuvitag_b/s132/config \
  $(PROJ_DIR)/occ/occ/OberonHAPCryptoP256 \
  $(PROJ_DIR)/ruuvitag_b/s132/ \
  $(PROJ_DIR)/occ/occ/OberonHAPCrypto \
  $(PROJ_DIR)/occ/occ/OberonHAPCrypto/include \
  $(PROJ_DIR)/occ/occ/OberonHAPCryptoP256/include \
  $(SDK_ROOT)/components/drivers_nrf/comp \
  $(SDK_ROOT)/components/drivers_nrf/twi_master \
  $(SDK_ROOT)/components/ble/ble_services/ble_ancs_c \
  $(SDK_ROOT)/components/ble/ble_services/ble_ias_c \
  $(SDK_ROOT)/components/libraries/pwm \
  $(SDK_ROOT)/external/tiny-AES128 \
  $(SDK_ROOT)/components/softdevice/s132/headers/nrf52 \
  $(SDK_ROOT)/components/libraries/bootloader/dfu/ \
  $(SDK_ROOT)/components/libraries/usbd/class/cdc/acm \
  $(SDK_ROOT)/components/libraries/usbd/class/hid/generic \
  $(SDK_ROOT)/components/libraries/usbd/class/msc \
  $(SDK_ROOT)/components/libraries/usbd/class/hid \
  $(SDK_ROOT)/components/libraries/eddystone \
  $(SDK_ROOT)/components/libraries/log \
  $(SDK_ROOT)/components/ble/ble_services/ble_gls \
  $(SDK_ROOT)/components/libraries/fstorage \
  $(SDK_ROOT)/components/drivers_nrf/i2s \
  $(SDK_ROOT)/components/libraries/gpiote \
  $(SDK_ROOT)/components/drivers_nrf/gpiote \
  $(SDK_ROOT)/components/drivers_nrf/common \
  $(SDK_ROOT)/components/ble/ble_advertising \
  $(SDK_ROOT)/components/drivers_nrf/adc \
  $(SDK_ROOT)/components/ble/ble_services/ble_bas_c \
  $(SDK_ROOT)/external/cifra_AES128-EAX \
  $(SDK_ROOT)/components/ble/ble_services/ble_hrs_c \
  $(SDK_ROOT)/components/libraries/queue \
  $(SDK_ROOT)/components/ble/ble_dtm \
  $(SDK_ROOT)/components/toolchain/cmsis/include \
  $(SDK_ROOT)/components/ble/ble_services/ble_rscs_c \
  $(SDK_ROOT)/components/drivers_nrf/uart \
  $(SDK_ROOT)/components/ble/common \
  $(SDK_ROOT)/components/ble/ble_services/ble_lls \
  $(SDK_ROOT)/components/drivers_nrf/wdt \
  $(SDK_ROOT)/components/ble/ble_services/ble_bas \
  $(SDK_ROOT)/components/libraries/experimental_section_vars \
  $(SDK_ROOT)/components/softdevice/s132/headers \
  $(SDK_ROOT)/components/ble/ble_services/ble_ans_c \
  $(SDK_ROOT)/components/libraries/slip \
  $(SDK_ROOT)/components/libraries/mem_manager \
  $(SDK_ROOT)/external/segger_rtt \
  $(SDK_ROOT)/components/libraries/usbd/class/cdc \
  $(SDK_ROOT)/components/drivers_nrf/hal \
  $(SDK_ROOT)/components/ble/ble_services/ble_nus_c \
  $(SDK_ROOT)/components/drivers_nrf/rtc \
  $(SDK_ROOT)/components/ble/ble_services/ble_ias \
  $(SDK_ROOT)/components/libraries/usbd/class/hid/mouse \
  $(SDK_ROOT)/components/drivers_nrf/ppi \
  $(SDK_ROOT)/components/ble/ble_services/ble_dfu \
  $(SDK_ROOT)/components/drivers_nrf/twis_slave \
  $(SDK_ROOT)/components \
  $(SDK_ROOT)/components/libraries/scheduler \
  $(SDK_ROOT)/components/ble/ble_services/ble_lbs \
  $(SDK_ROOT)/components/ble/ble_services/ble_hts \
  $(SDK_ROOT)/components/ble/ble_services/ble_dfu \
  $(SDK_ROOT)/components/ble/ble_services/ble_dis \
  $(SDK_ROOT)/components/drivers_nrf/delay \
  $(SDK_ROOT)/components/libraries/crc16 \
  $(SDK_ROOT)/components/drivers_nrf/timer \
  $(SDK_ROOT)/components/libraries/util \
  $(SDK_ROOT)/components/drivers_nrf/pwm \
  $(SDK_ROOT)/components/libraries/csense_drv \
  $(SDK_ROOT)/components/libraries/csense \
  $(SDK_ROOT)/components/drivers_nrf/rng \
  $(SDK_ROOT)/components/libraries/low_power_pwm \
  $(SDK_ROOT)/components/libraries/hardfault \
  $(SDK_ROOT)/components/ble/ble_services/ble_cscs \
  $(SDK_ROOT)/components/libraries/uart \
  $(SDK_ROOT)/components/libraries/hci \
  $(SDK_ROOT)/components/libraries/usbd/class/hid/kbd \
  $(SDK_ROOT)/components/drivers_nrf/spi_slave \
  $(SDK_ROOT)/components/drivers_nrf/lpcomp \
  $(SDK_ROOT)/components/libraries/timer \
  $(SDK_ROOT)/components/drivers_nrf/power \
  $(SDK_ROOT)/components/libraries/usbd/config \
  $(SDK_ROOT)/components/toolchain \
  $(SDK_ROOT)/components/libraries/led_softblink \
  $(SDK_ROOT)/components/drivers_nrf/qdec \
  $(SDK_ROOT)/components/ble/ble_services/ble_cts_c \
  $(SDK_ROOT)/components/drivers_nrf/spi_master \
  $(SDK_ROOT)/components/ble/ble_services/ble_nus \
  $(SDK_ROOT)/components/ble/ble_services/ble_hids \
  $(SDK_ROOT)/components/drivers_nrf/pdm \
  $(SDK_ROOT)/components/libraries/crc32 \
  $(SDK_ROOT)/components/libraries/usbd/class/audio \
  $(SDK_ROOT)/components/libraries/sensorsim \
  $(SDK_ROOT)/components/ble/peer_manager \
  $(SDK_ROOT)/components/drivers_nrf/swi \
  $(SDK_ROOT)/components/ble/ble_services/ble_tps \
  $(SDK_ROOT)/components/ble/ble_services/ble_dis \
  $(SDK_ROOT)/components/device \
  $(SDK_ROOT)/components/ble/nrf_ble_qwr \
  $(SDK_ROOT)/components/libraries/button \
  $(SDK_ROOT)/components/libraries/usbd \
  $(SDK_ROOT)/components/drivers_nrf/saadc \
  $(SDK_ROOT)/components/ble/ble_services/ble_lbs_c \
  $(SDK_ROOT)/components/ble/ble_services/ble_escs \
  $(SDK_ROOT)/components/ble/ble_racp \
  $(SDK_ROOT)/components/toolchain/gcc \
  $(SDK_ROOT)/components/libraries/fds \
  $(SDK_ROOT)/components/libraries/twi \
  $(SDK_ROOT)/components/drivers_nrf/clock \
  $(SDK_ROOT)/components/ble/ble_services/ble_rscs \
  $(SDK_ROOT)/components/drivers_nrf/usbd \
  $(SDK_ROOT)/components/softdevice/common/softdevice_handler \
  $(SDK_ROOT)/components/ble/ble_services/ble_hrs \
  $(SDK_ROOT)/components/libraries/log/src \
  $(SDK_ROOT)/components/nfc/t2t_lib/hal_t2t \
  $(SDK_ROOT)/components/nfc/ndef/text \
  $(SDK_ROOT)/components/nfc/ndef/generic/message \
  $(SDK_ROOT)/components/nfc/ndef/generic/record \
  $(SDK_ROOT)/components/nfc/t2t_lib \
  $(SDK_ROOT)/components/nfc/ndef/text \

# Libraries common to all targets
LIB_FILES += \
  $(PROJ_DIR)/occ/occ_lib_nrf52.a \
  $(SDK_ROOT)/components/nfc/t2t_lib/nfc_t2t_lib_gcc.a 

# C flags common to all targets
CFLAGS += -DNRF52
CFLAGS += -DNRF52_PAN_64
CFLAGS += -DSOFTDEVICE_PRESENT
CFLAGS += -DBOARD_CUSTOM
CFLAGS += -DBOARD_RUUVITAG_B
CFLAGS += -DNRF52832
CFLAGS += -DNRF_LOG_USES_RTT=1
CFLAGS += -DNRF_DFU_SETTINGS_VERSION=1
CFLAGS += -DNRF52_PAN_58
CFLAGS += -DNRF52_PAN_54
CFLAGS += -DNRF52_PAN_31
CFLAGS += -DNRF52_PAN_51
CFLAGS += -DNRF52_PAN_36
CFLAGS += -DUART_DEBUG
CFLAGS += -DCONFIG_GPIO_AS_PINRESET
CFLAGS += -DNRF52_PAN_12
CFLAGS += -DBLE_STACK_SUPPORT_REQD
CFLAGS += -DNRF52_PAN_15
CFLAGS += -DNRF_SD_BLE_API_VERSION=3
CFLAGS += -DSWI_DISABLE0
CFLAGS += -DNRF52_PAN_20
CFLAGS += -DNRF52_PAN_55
CFLAGS += -DS132
CFLAGS += -mcpu=cortex-m4
CFLAGS += -mthumb -mabi=aapcs
#-Werror
CFLAGS +=  -Wall -Werror -O3 -g3
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# keep every function in separate section, this allows linker to discard unused ones
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin --short-enums 

# C++ flags common to all targets
CXXFLAGS += \

# Assembler flags common to all targets
ASMFLAGS += -x assembler-with-cpp
ASMFLAGS += -DNRF52
ASMFLAGS += -DNRF52_PAN_64
ASMFLAGS += -DSOFTDEVICE_PRESENT
ASMFLAGS += -DBOARD_PCA10040
ASMFLAGS += -DNRF52832
ASMFLAGS += -DNRF_LOG_USES_RTT=1
ASMFLAGS += -DNRF_DFU_SETTINGS_VERSION=1
ASMFLAGS += -DNRF52_PAN_58
ASMFLAGS += -DNRF52_PAN_54
ASMFLAGS += -DNRF52_PAN_31
ASMFLAGS += -DNRF52_PAN_51
ASMFLAGS += -DNRF52_PAN_36
ASMFLAGS += -DUART_DEBUG
ASMFLAGS += -DCONFIG_GPIO_AS_PINRESET
ASMFLAGS += -DNRF52_PAN_12
ASMFLAGS += -DBLE_STACK_SUPPORT_REQD
ASMFLAGS += -DNRF52_PAN_15
ASMFLAGS += -DNRF_SD_BLE_API_VERSION=3
ASMFLAGS += -DSWI_DISABLE0
ASMFLAGS += -DNRF52_PAN_20
ASMFLAGS += -DNRF52_PAN_55
ASMFLAGS += -DS132

# Linker flags
LDFLAGS += -mthumb -mabi=aapcs -L $(TEMPLATE_PATH) -T$(LINKER_SCRIPT)
LDFLAGS += -mcpu=cortex-m4
LDFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# let linker to dump unused sections
LDFLAGS += -Wl,--gc-sections
# use newlib in nano version
LDFLAGS += --specs=nano.specs -lc -lnosys

.PHONY: $(TARGETS) default all clean

# Default target - first one defined
default: eddystone

# Print all targets that can be built
help:
	@echo following targets are available:
	@echo   eddystone

TEMPLATE_PATH := $(SDK_ROOT)/components/toolchain/gcc

include $(TEMPLATE_PATH)/Makefile.common
$(foreach target, $(TARGETS), $(call define_target, $(target)))
