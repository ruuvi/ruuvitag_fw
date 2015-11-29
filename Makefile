#
#
#

TOP := `pwd`

SDK_VERSION := 0.9.2_dbc28c9
SDK_URL     := https://developer.nordicsemi.com/nRF52_SDK/nRF52_SDK_v0.x.x
SDK_FILE    := nRF52_SDK_$(SDK_VERSION).zip

SDK_DIR     := $(basename $(SDK_FILE))
SDK_HOME    := $(TOP)/$(SDK_DIR)

export SDK_HOME

.PHONY: all download_sdk bootstrap fw bootloader

all: bootstrap fw bootloader

bootstrap: $(SDK_FILE)
	@echo SDK_HOME = ${SDK_HOME}

$(SDK_FILE): download_sdk
	@if [ ! -d $(SDK_DIR) ]; then \
	 unzip -q -d $(SDK_DIR) $(SDK_FILE); fi

download_sdk:
	@if [ ! -f $(SDK_FILE) ] ;then echo downloading SDK... ; curl -0 $(SDK_URL)/$(SDK_FILE); fi

fw:
	@echo build FW
	$(MAKE) -C fw/ruuvitag_revb1/armgcc

bootloader:
	@echo build bootloader
	$(MAKE) -C bootloader/ruuvitag_revb1/dual_bank_ble_s132/armgcc

clean:
	@echo cleaning FW build files…
	$(MAKE) -C fw/ruuvitag_revb1/armgcc clean
	@echo cleaning bootloader build files…
	$(MAKE) -C bootloader/ruuvitag_revb1/dual_bank_ble_s132/armgcc clean