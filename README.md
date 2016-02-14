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

`nrfutil dfu genpkg --application fw/ruuvitag_b1/s132/armgcc/_build/ruuvitag_b1_fw.hex --application-version 0xffff --dev-revision 0xff --dev-type 0xff --sd-req 0xfffe /Users/lauri/Dropbox/RuuviTag_FW.zip`

Or if want to create distribution package that includes both bootloader and fw:

`nrfutil dfu genpkg --bootloader bootloader/ruuvitag_b1/dual_bank_ble_s132/armgcc/_build/ruuvitag_b1_bootloader.hex --application fw/ruuvitag_b1/s132/armgcc/_build/ruuvitag_b1_fw.hex --application-version 0xffff --dev-revision 0xff --dev-type 0xff --sd-req 0xfffe /Users/lauri/Dropbox/RuuviTag_Bootloader_and_FW.zip`

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

`J-Link>loadfile nRF5_SDK_11.0.0-2.alpha_bc3f6a0/components/softdevice/s132/hex/s132_nrf52_2.0.0-7.alpha_softdevice.hex`

If the J-Link asks to verify interface and speed, `swd` and `1000` (kHz) should be fine.

After the SoftDevice is flashed successfully, flash the bootloader:

`J-Link>loadfile bootloader/ruuvitag_b2/dual_bank_ble_s132/armgcc/_build/ruuvitag_b2_bootloader.hex`

After this no cables are needed, ever (unless the device needs to be rescued for some reason)! From now on, the FW (and/or the bootloader and/or the SoftDevice) can be updated Over-The-Air using Nordic's nRF Toolbox:

https://www.nordicsemi.com/eng/Products/nRFready-Demo-Apps/nRF-Toolbox-App

https://github.com/NordicSemiconductor/Android-nRF-Toolbox

https://github.com/NordicSemiconductor/IOS-nRF-Toolbox

# This is probably what you're after:

1. Flash the SoftDevice protocol stack
2. Compile the bootloader and the firmware
3. Flash the bootloader
4. Reset the device
5. Create .zip distribution package (that includes at least the application)
6. Install nRF Toolbox (Android/iOS)
7. Hit DFU OTA!
8. After completed, hit it again! And again! No more cables needed ^^
9. Now you can update SoftDevice and/or bootloader and/or application using DFU OTA. Cool, huh?

[![RuuviTag Factsheet](https://github.com/ruuvi/ruuvi_brand/blob/master/ruuvitag_factsheet_2015/ruuvitag_factsheet_2015.jpg)](http://ruuvi.com)
