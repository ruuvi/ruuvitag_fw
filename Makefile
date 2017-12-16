#
#
#

ifeq ($(OS),Windows_NT)
	TOP := %cd%
else
	TOP := `pwd`
	FILEID = `gdrive list --query "name contains 'weather_station-test.zip'"|sed '2!d' |sed 's/ weather.*//'`
endif

SDK_VERSION := 12.3.0_d7731ad
SDK_URL     := https://developer.nordicsemi.com/nRF5_SDK/nRF5_SDK_v12.x.x
SDK_FILE    := nRF5_SDK_$(SDK_VERSION).zip

SDK_DIR     := $(basename $(SDK_FILE))
SDK_HOME    := $(TOP)/$(SDK_DIR)

ifeq ($(OS),Windows_NT)
	DOWNLOAD_CMD ?= powershell curl -o
	UNZIP_CMD ?= powershell Expand-Archive -DestinationPath .
else
	DOWNLOAD_CMD ?= curl -o
	UNZIP_CMD ?= unzip -q
	#UNZIP_CMD ?= unzip -q -d
endif

export $(SDK_HOME)

.PHONY: all bootstrap fw bootloader

all: bootstrap fw bootloader

bootstrap: $(SDK_FILE) $(SDK_DIR) $(SDK_DIR)/external/micro-ecc/micro-ecc
	@echo SDK_HOME = ${SDK_HOME}

$(SDK_DIR):
	$(UNZIP_CMD) $(SDK_FILE)
	$(call patch_sdk_$(SDK_VERSION))

$(SDK_FILE):
	@echo downloading SDK zip...
	$(DOWNLOAD_CMD) $(SDK_FILE) $(SDK_URL)/$(SDK_FILE)

$(SDK_DIR)/external/micro-ecc/micro-ecc:
	git clone https://github.com/kmackay/micro-ecc.git $(SDK_DIR)/external/micro-ecc/micro-ecc
	$(MAKE) -C $(SDK_DIR)/external/micro-ecc/nrf52_armgcc/armgcc



fw:
	@echo build FW
	git submodule sync
	git submodule update --init --recursive
	$(MAKE) -C ruuvi_examples/ble_app_beacon/ruuvitag_b/s132/armgcc
	$(MAKE) -C ruuvi_examples/eddystone/ruuvitag_b/s132/armgcc
	$(MAKE) -C ruuvi_examples/test_drivers/ruuvitag_b/s132/armgcc
	$(MAKE) -C ruuvi_examples/ruuvi_firmware/ruuvitag_b/s132/armgcc


bootloader:
	@echo build bootloader
	$(MAKE) -C bootloader/ruuvitag_b_debug/armgcc
	$(MAKE) -C bootloader/ruuvitag_b_production/armgcc

clean:
	@echo cleaning B build files…
	git submodule sync
	git submodule update --init --recursive
	$(MAKE) -C ruuvi_examples/ble_app_beacon/ruuvitag_b/s132/armgcc clean
	$(MAKE) -C ruuvi_examples/eddystone/ruuvitag_b/s132/armgcc clean
	$(MAKE) -C ruuvi_examples/test_drivers/ruuvitag_b/s132/armgcc clean
	$(MAKE) -C ruuvi_examples/ruuvi_firmware/ruuvitag_b/s132/armgcc clean
	$(MAKE) -C bootloader/ruuvitag_b_debug/armgcc clean
	$(MAKE) -C bootloader/ruuvitag_b_production/armgcc clean

distro:
	@echo Prepare distribution…
	rm -rf builds/distribution_packages/sdk12/ruuvitag_firmware-test.zip
	nrfutil pkg generate --debug-mode --application ruuvi_examples/weather_station/ruuvitag_b3/s132/armgcc/_build/weather_station.hex --key-file keys/ruuvi_open_private.pem builds/distribution_packages/sdk12/weather_station-test.zip
	@echo Uploading $(FILEID) …
	gdrive update $(FILEID) builds/distribution_packages/sdk12/weather_station-test.zip
