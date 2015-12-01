# ruuvitag_fw nrf52 branch

This repo has:

* Bootloader
* Firmware

### Prerequisites (to compile):

* Download and install GCC https://launchpad.net/gcc-arm-embedded/+download

Extract the GCC tarball. Other destinations are also ok, but this one is used often:
`sudo mkdir -p /usr/local && cd /usr/local && sudo tar xjf ~/Downloads/gcc-arm-none-eabi-4_xxxxxxxx.tar.bz2`

To add to path create a file `/etc/paths.d/arm-gcc`:

`/usr/local/gcc-arm-none-eabi-4_9-2015q3/bin`

# Compiling

`make` downloads Nordic Semiconductor's nRF52 SDK and extracts it. After that builds the bootloader and the FW.
`make clean` cleans the build directories.

For more help, request an invite to Ruuvi's Slack channel!

http://ruuvi.com/blog/ruuvi-slack-com.html



# TODO:

At the moment SDK requires some patching to compile example fw correctly:

1) ERROR: ...........\components\libraries\bootloader_dfu\dfu_app_handler.c(153): error: #136: struct "<unnamed>" has no field "BOOTLOADERADDR". You have to modify line 146 and 153 in components\libraries\bootloader_dfu\dfu_app_handler.c. In both those lines you should replace `NRF_UICR->BOOTLOADERADDR` with `*(uint32_t *)(0x10001014)`.
https://devzone.nordicsemi.com/question/56723/dfu-on-nrf52/

2) "Set the maximum number of characteristic client descriptors in the file device_manager_cnfg.h (located in <InstallFolder>\components\ble\device_manager\config):"
`#define DM_GATT_CCCD_COUNT               4`

# DFU distribution .zip package

`nrfutil` knows how to create .zip distribution package (for DFU OTA).
Instructions how to install nrfutil (on OS X):

`git clone https://github.com/NordicSemiconductor/pc-nrfutil.git`
`cd pc-nrfutil`
`curl -O https://bootstrap.pypa.io/get-pip.py`
`sudo python get-pip.py`
`sudo pip install -r requirements.txt`
`sudo python setup.py install`
`nrfutil version`

How to use it:

`nrfutil dfu genpkg --application nrf52832_xxaa_s132.hex --application-version 0xffff --dev-revision 0xff --dev-type 0xff --sd-req 0xfffe DFUTEST.zip
Zip created at DFUTEST.zip`
