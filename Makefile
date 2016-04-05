#
#
#

TOP := `pwd`

SDK_VERSION := 11.0.0_89a8197
SDK_URL     := https://developer.nordicsemi.com/nRF5_SDK/nRF5_SDK_v11.x.x
SDK_FILE    := nRF5_SDK_$(SDK_VERSION).zip

SDK_DIR     := $(basename $(SDK_FILE))
SDK_HOME    := $(TOP)/$(SDK_DIR)

DOWNLOAD_CMD ?= curl -O

export SDK_HOME

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



#fw:
#	@echo build FW
#	$(MAKE) -C fw/ruuvitag_b2/s132/armgcc

bootloader:
	@echo build bootloader
	$(MAKE) -C bootloader/ruuvitag_b2/dual_bank_ble_s132/armgcc

clean:
#	@echo cleaning FW build files…
#	$(MAKE) -C fw/ruuvitag_b2/s132/armgcc clean
	@echo cleaning bootloader build files…
	$(MAKE) -C bootloader/ruuvitag_b2/dual_bank_ble_s132/armgcc clean
