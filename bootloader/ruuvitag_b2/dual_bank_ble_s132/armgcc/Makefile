PROJECT_NAME := dfu_dual_bank_ble_s132_ruuvitag_b2

export OUTPUT_FILENAME
#MAKEFILE_NAME := $(CURDIR)/$(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))
MAKEFILE_NAME := $(MAKEFILE_LIST)
MAKEFILE_DIR := $(dir $(MAKEFILE_NAME) ) 
SDK = ../../../../nRF5_SDK_11.0.0_89a8197

TEMPLATE_PATH = $(SDK)/components/toolchain/gcc
ifeq ($(OS),Windows_NT)
include $(TEMPLATE_PATH)/Makefile.windows
else
include $(TEMPLATE_PATH)/Makefile.posix
endif

MK := mkdir
RM := rm -rf

#echo suspend
ifeq ("$(VERBOSE)","1")
NO_ECHO := 
else
NO_ECHO := @
endif

# Toolchain commands
CC              := '$(GNU_PREFIX)-gcc'
AS              := '$(GNU_PREFIX)-as'
AR              := '$(GNU_PREFIX)-ar' -r
LD              := '$(GNU_PREFIX)-ld'
NM              := '$(GNU_PREFIX)-nm'
OBJDUMP         := '$(GNU_PREFIX)-objdump'
OBJCOPY         := '$(GNU_PREFIX)-objcopy'
SIZE            := '$(GNU_PREFIX)-size'

#function for removing duplicates in a list
remduplicates = $(strip $(if $1,$(firstword $1) $(call remduplicates,$(filter-out $(firstword $1),$1))))

#source common to all targets
C_SOURCE_FILES += \
$(SDK)/components/libraries/util/app_error.c \
$(SDK)/components/libraries/scheduler/app_scheduler.c \
$(SDK)/components/libraries/timer/app_timer.c \
$(SDK)/components/libraries/timer/app_timer_appsh.c \
$(SDK)/components/libraries/bootloader_dfu/bootloader.c \
$(SDK)/components/libraries/bootloader_dfu/bootloader_settings.c \
$(SDK)/components/libraries/bootloader_dfu/bootloader_util.c \
$(SDK)/components/libraries/crc16/crc16.c \
$(SDK)/components/libraries/bootloader_dfu/dfu_dual_bank.c \
$(SDK)/components/libraries/bootloader_dfu/dfu_init_template.c \
$(SDK)/components/libraries/bootloader_dfu/dfu_transport_ble.c \
$(SDK)/components/libraries/hci/hci_mem_pool.c \
$(SDK)/components/libraries/util/nrf_assert.c \
$(SDK)/components/drivers_nrf/delay/nrf_delay.c \
$(SDK)/components/drivers_nrf/common/nrf_drv_common.c \
$(SDK)/components/drivers_nrf/pstorage/pstorage_raw.c \
../../../dfu_ble_svc.c \
../../../main.c \
$(SDK)/components/ble/common/ble_advdata.c \
$(SDK)/components/ble/common/ble_conn_params.c \
$(SDK)/components/ble/ble_services/ble_dfu/ble_dfu.c \
$(SDK)/components/ble/common/ble_srv_common.c \
$(SDK)/components/toolchain/system_nrf52.c \
$(SDK)/components/softdevice/common/softdevice_handler/softdevice_handler.c \
$(SDK)/components/softdevice/common/softdevice_handler/softdevice_handler_appsh.c \
$(SDK)/components/libraries/util/app_util_platform.c \

#assembly files common to all targets
ASM_SOURCE_FILES  = $(SDK)/components/toolchain/gcc/gcc_startup_nrf52.s

#includes common to all targets
INC_PATHS  = -I../../../../board_configs
INC_PATHS += -I../../../config
INC_PATHS += -I$(SDK)/components/libraries/util
INC_PATHS += -I$(SDK)/components/libraries/timer
INC_PATHS += -I$(SDK)/components/softdevice/s132/headers
INC_PATHS += -I$(SDK)/components/ble/common
INC_PATHS += -I$(SDK)/components/drivers_nrf/common
INC_PATHS += -I$(SDK)/examples/bsp
INC_PATHS += -I$(SDK)/components/drivers_nrf/config
INC_PATHS += -I$(SDK)/components/toolchain/gcc
INC_PATHS += -I$(SDK)/components/libraries/scheduler
INC_PATHS += -I$(SDK)/components/drivers_nrf/pstorage
INC_PATHS += -I$(SDK)/components/libraries/bootloader_dfu/ble_transport
INC_PATHS += -I$(SDK)/components/softdevice/common/softdevice_handler
INC_PATHS += -I$(SDK)/components/device
INC_PATHS += -I$(SDK)/components/libraries/hci
INC_PATHS += -I$(SDK)/components/toolchain
INC_PATHS += -I$(SDK)/components/drivers_nrf/delay
INC_PATHS += -I$(SDK)/components/libraries/crc16
INC_PATHS += -I$(SDK)/components/ble/ble_services/ble_dfu
INC_PATHS += -I$(SDK)/components/drivers_nrf/hal
INC_PATHS += -I$(SDK)/components/softdevice/s132/headers/nrf52
INC_PATHS += -I$(SDK)/components/libraries/bootloader_dfu
INC_PATHS += -I$(SDK)/components/toolchain/CMSIS/Include
INC_PATHS += -I$(SDK)/components/libraries/util/app_util_platform

OBJECT_DIRECTORY = _build
LISTING_DIRECTORY = $(OBJECT_DIRECTORY)
OUTPUT_BINARY_DIRECTORY = $(OBJECT_DIRECTORY)

# Sorting removes duplicates
BUILD_DIRECTORIES := $(sort $(OBJECT_DIRECTORY) $(OUTPUT_BINARY_DIRECTORY) $(LISTING_DIRECTORY) )

#flags common to all targets
CFLAGS  = -DNRF52
CFLAGS += -DBOARD_CUSTOM
CFLAGS += -DBOARD_RUUVITAG_B2
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
CFLAGS += -D__HEAP_SIZE=0
CFLAGS += -DS132
CFLAGS += -DCONFIG_GPIO_AS_PINRESET
CFLAGS += -DBLE_STACK_SUPPORT_REQD
CFLAGS += -DBSP_DEFINES_ONLY
CFLAGS += -DSWI_DISABLE0
CFLAGS += -DNRF52_PAN_20
CFLAGS += -DNRF52_PAN_64
CFLAGS += -DSOFTDEVICE_PRESENT
CFLAGS += -DNRF52_PAN_62
CFLAGS += -DNRF52_PAN_63
CFLAGS += -mcpu=cortex-m4
CFLAGS += -mthumb -mabi=aapcs --std=gnu99
CFLAGS += -Wall -Werror -Os -g3
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# keep every function in separate section. This will allow linker to dump unused functions
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin --short-enums

# keep every function in separate section. This will allow linker to dump unused functions
LDFLAGS += -Xlinker -Map=$(LISTING_DIRECTORY)/$(OUTPUT_FILENAME).map
LDFLAGS += -mthumb -mabi=aapcs -L $(TEMPLATE_PATH) -T$(LINKER_SCRIPT)
LDFLAGS += -mcpu=cortex-m4
LDFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# let linker to dump unused sections
LDFLAGS += -Wl,--gc-sections
# use newlib in nano version
LDFLAGS += --specs=nano.specs -lc -lnosys

# Assembler flags
ASMFLAGS += -x assembler-with-cpp
ASMFLAGS += -DNRF52
ASMFLAGS += -DBOARD_CUSTOM
ASMFLAGS += -DBOARD_RUUVITAG_B2
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
ASMFLAGS += -D__HEAP_SIZE=0
ASMFLAGS += -DS132
ASMFLAGS += -DCONFIG_GPIO_AS_PINRESET
ASMFLAGS += -DBLE_STACK_SUPPORT_REQD
ASMFLAGS += -DBSP_DEFINES_ONLY
ASMFLAGS += -DSWI_DISABLE0
ASMFLAGS += -DNRF52_PAN_20
ASMFLAGS += -DNRF52_PAN_64
ASMFLAGS += -DSOFTDEVICE_PRESENT
ASMFLAGS += -DNRF52_PAN_62
ASMFLAGS += -DNRF52_PAN_63
#default target - first one defined
default: clean ruuvitag_b2_bootloader

#building all targets
all: clean
	$(NO_ECHO)$(MAKE) -f $(MAKEFILE_NAME) -C $(MAKEFILE_DIR) -e cleanobj
	$(NO_ECHO)$(MAKE) -f $(MAKEFILE_NAME) -C $(MAKEFILE_DIR) -e ruuvitag_b2_bootloader

#target for printing all targets
help:
	@echo following targets are available:
	@echo 	ruuvitag_b2_bootloader


C_SOURCE_FILE_NAMES = $(notdir $(C_SOURCE_FILES))
C_PATHS = $(call remduplicates, $(dir $(C_SOURCE_FILES) ) )
C_OBJECTS = $(addprefix $(OBJECT_DIRECTORY)/, $(C_SOURCE_FILE_NAMES:.c=.o) )

ASM_SOURCE_FILE_NAMES = $(notdir $(ASM_SOURCE_FILES))
ASM_PATHS = $(call remduplicates, $(dir $(ASM_SOURCE_FILES) ))
ASM_OBJECTS = $(addprefix $(OBJECT_DIRECTORY)/, $(ASM_SOURCE_FILE_NAMES:.s=.o) )

vpath %.c $(C_PATHS)
vpath %.s $(ASM_PATHS)

OBJECTS = $(C_OBJECTS) $(ASM_OBJECTS)

ruuvitag_b2_bootloader: OUTPUT_FILENAME := ruuvitag_b2_bootloader
ruuvitag_b2_bootloader: LINKER_SCRIPT=../../../dfu_gcc_nrf52.ld
ruuvitag_b2_bootloader: $(BUILD_DIRECTORIES) $(OBJECTS)
	@echo Linking target: $(OUTPUT_FILENAME).out
	$(NO_ECHO)$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out
	$(NO_ECHO)$(MAKE) -f $(MAKEFILE_NAME) -C $(MAKEFILE_DIR) -e finalize

## Create build directories
$(BUILD_DIRECTORIES):
	echo $(MAKEFILE_NAME)
	$(MK) $@

# Create objects from C SRC files
$(OBJECT_DIRECTORY)/%.o: %.c
	@echo Compiling file: $(notdir $<)
	$(NO_ECHO)$(CC) $(CFLAGS) $(INC_PATHS) -c -o $@ $<

# Assemble files
$(OBJECT_DIRECTORY)/%.o: %.s
	@echo Compiling file: $(notdir $<)
	$(NO_ECHO)$(CC) $(ASMFLAGS) $(INC_PATHS) -c -o $@ $<


# Link
$(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out: $(BUILD_DIRECTORIES) $(OBJECTS)
	@echo Linking target: $(OUTPUT_FILENAME).out
	$(NO_ECHO)$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out


## Create binary .bin file from the .out file
$(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).bin: $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out
	@echo Preparing: $(OUTPUT_FILENAME).bin
	$(NO_ECHO)$(OBJCOPY) -O binary $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).bin

## Create binary .hex file from the .out file
$(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).hex: $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out
	@echo Preparing: $(OUTPUT_FILENAME).hex
	$(NO_ECHO)$(OBJCOPY) -O ihex $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).hex

finalize: genbin genhex echosize

genbin:
	@echo Preparing: $(OUTPUT_FILENAME).bin
	$(NO_ECHO)$(OBJCOPY) -O binary $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).bin

## Create binary .hex file from the .out file
genhex: 
	@echo Preparing: $(OUTPUT_FILENAME).hex
	$(NO_ECHO)$(OBJCOPY) -O ihex $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).hex

echosize:
	-@echo ''
	$(NO_ECHO)$(SIZE) $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out
	-@echo ''

clean:
	$(RM) $(BUILD_DIRECTORIES)

cleanobj:
	$(RM) $(BUILD_DIRECTORIES)/*.o

flash: $(MAKECMDGOALS)
	@echo Flashing: $(OUTPUT_BINARY_DIRECTORY)/$<.hex
	nrfjprog --program $(OUTPUT_BINARY_DIRECTORY)/$<.hex -f nrf52  --chiperase
	nrfjprog --reset

## Flash softdevice