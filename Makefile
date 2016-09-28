#
#
#

TOP := `pwd`

SDK_VERSION := 12.0.0_12f24da
SDK_URL     := https://developer.nordicsemi.com/nRF5_SDK/nRF5_SDK_v12.x.x
SDK_FILE    := nRF5_SDK_$(SDK_VERSION).zip

SDK_DIR     := $(basename $(SDK_FILE))
SDK_HOME    := $(TOP)/$(SDK_DIR)

DOWNLOAD_CMD ?= curl -O

export $(SDK_HOME)

.PHONY: all bootstrap fw bootloader

all: bootstrap fw bootloader

bootstrap: $(SDK_FILE) $(SDK_DIR)
	@echo SDK_HOME = ${SDK_HOME}

$(SDK_DIR):
	unzip -q -d $(SDK_DIR) $(SDK_FILE)
	$(call patch_sdk_$(SDK_VERSION))

$(SDK_FILE):
	@echo downloading SDK zip...
	$(DOWNLOAD_CMD) $(SDK_URL)/$(SDK_FILE)



fw:
	@echo build FW
	$(MAKE) -C ruuvi_examples/ble_app_beacon/ruuvitag_b3/s132/armgcc

bootloader:
	@echo build bootloader
	$(MAKE) -C bootloader/ruuvitag_b3_debug/armgcc
	$(MAKE) -C bootloader/ruuvitag_b3_production/armgcc

clean:
	@echo cleaning B3 build files…
	$(MAKE) -C ruuvi_examples/ble_app_beacon/ruuvitag_b3/s132/armgcc clean
	$(MAKE) -C bootloader/ruuvitag_b3_debug/armgcc clean
	$(MAKE) -C bootloader/ruuvitag_b3_production/armgcc clean
