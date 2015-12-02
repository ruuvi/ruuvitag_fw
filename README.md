# RuuviTag nRF52 Bootloader & Firmware

This repo has at the moment:

* Bootloader project (unmodified dual_bank_ble_s132 bootloader example project, outputs .hex)
* Firmware project (ble_app_template project with added device manager support + dfu ota support, outputs .hex)

Instructions below are tested using OS X, but basically any Unix distribution should be fine. If you've compiled and flashed successfully (or unsuccessfully), please identify yourself on our Slack :)

### Prerequisites (to compile):

* Download and install GCC https://launchpad.net/gcc-arm-embedded/+download

Extract the GCC tarball. Other destinations are also ok, but this one is used often:
`sudo mkdir -p /usr/local && cd /usr/local && sudo tar xjf ~/Downloads/gcc-arm-none-eabi-4_xxxxxxxx.tar.bz2`

To add to path create a file `/etc/paths.d/arm-gcc` with content:

`/usr/local/gcc-arm-none-eabi-4_9-2015q3/bin`

### Prerequisites (to create DFU distribution .zip packages)

Instructions how to install (on OS X):

`git clone https://github.com/NordicSemiconductor/pc-nrfutil.git`

`cd pc-nrfutil`

`curl -O https://bootstrap.pypa.io/get-pip.py`

`sudo python get-pip.py`

`sudo pip install -r requirements.txt`

`sudo python setup.py install`

`nrfutil version`

How to use it:

`nrfutil dfu genpkg --application nrf52832_xxaa_s132.hex --application-version 0xffff --dev-revision 0xff --dev-type 0xff --sd-req 0xfffe DFUTEST.zip``

`Zip created at DFUTEST.zip`

# Compiling

`make` downloads Nordic Semiconductor's nRF52 SDK and extracts it. After that builds the bootloader and the FW.
`make clean` cleans the build directories.

For more help, request an invite to Ruuvi's Slack channel!

http://ruuvi.com/blog/ruuvi-slack-com.html

# Flashing

If the device is empty (no SoftDevice S132 + bootloader flashed), you need to flash using SWD interface. The easiest way is to use nRF52 development kit (PCA10036 or PCA10040) with embedded Segger. Steps:

Download and install latest J-Link https://www.segger.com/jlink-software.html

Start the J-Link from command line by typing:

`JLinkExe -device nrf52`

SoftDevice is Nordic Semiconductor's Bluetooth Smart (or ANT) protocol stack. Sources are super secret, but the latest version is always bundled with the SDK. So, let's flash it:

`J-Link>loadfile nRF52_SDK_0.9.2_dbc28c9/components/softdevice/s132/hex/s132_nrf52_1.0.0-3.alpha_softdevice.hex`

If the J-Link asks to verify interface and speed, `swd` and `1000` (kHz) should be fine.

After the SoftDevice is flashed successfully, flash the bootloader:

`J-Link>loadfile bootloader/ruuvitag_revb1/dual_bank_ble_s132/armgcc/_build/ruuvitag_revb1_bootloader.hex`

After this no cables are needed, ever (unless the device needs to be rescued for some reason)! Fow now on, the FW (and/or the bootloader and/or the SoftDevice) can be updated Over-The-Air using Nordic's nRF Toolbox:

https://www.nordicsemi.com/eng/Products/nRFready-Demo-Apps/nRF-Toolbox-App

https://github.com/NordicSemiconductor/Android-nRF-Toolbox

https://github.com/NordicSemiconductor/IOS-nRF-Toolbox

# This is probably what you're after:

* Flash SoftDevice protocol stack
* Compile bootloader and firmware
* Flash the bootloader
* Reset the device
* Create .zip distribution package (that includes at least the application)
* Install nRF Toolbox (Android/iOS/WP)
* Hit DFU OTA!
* After completed, hit it again! And again! No more cables needed ^^
* Now you can update SoftDevice and/or bootloader and/or application using DFU OTA. Cool, huh?

# TODO:

At the moment SDK requires some patching to compile example FW project correctly:

1) ERROR: ...........\components\libraries\bootloader_dfu\dfu_app_handler.c(153): error: #136: struct "<unnamed>" has no field "BOOTLOADERADDR". You have to modify line 146 and 153 in components\libraries\bootloader_dfu\dfu_app_handler.c. In both those lines you should replace `NRF_UICR->BOOTLOADERADDR` with `*(uint32_t *)(0x10001014)`.
https://devzone.nordicsemi.com/question/56723/dfu-on-nrf52/

2) "Set the maximum number of characteristic client descriptors in the file device_manager_cnfg.h (located in <InstallFolder>\components\ble\device_manager\config):"
`#define DM_GATT_CCCD_COUNT               4`
