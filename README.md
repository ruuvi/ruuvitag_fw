# RuuviTag nRF52 Bootloader & Example firmware projects
[![RuuviTag](http://ruuvitag.com/assets/images/fb_ruuvitag.jpg)](http://ruuvitag.com)

This repo has at the moment:

* Bootloader project (almost unmodified dual_bank_ble_s132 bootloader example project, outputs .hex)
* Multiple example firmware projects

Instructions below are tested using OS X, but basically any Unix distribution (or even Windows) should be fine. If you've compiled and flashed successfully (or unsuccessfully), please identify yourself on our Slack :)

We also host some ready binaries so it's not necessary to setup a development environment if you would be happy to use those. So, please check a `builds` directory first. If you would like to modify the firmware code, continue reading:

### Prerequisites (to compile):

* Download and install GCC https://launchpad.net/gcc-arm-embedded/+download

Extract the GCC tarball. Other destinations are also ok, but this one is used often:
`sudo mkdir -p /usr/local && cd /usr/local && sudo tar xjf ~/Downloads/gcc-arm-none-eabi-4_xxxxxxxx.tar.bz2`

Toolchain path is defined in SDK Makefile, so adding the gcc-arm-none-eabi binaries to path is unnecessary.

### Prerequisites (to create DFU distribution .zip packages)

Instructions how to install (on OS X):

`git clone https://github.com/NordicSemiconductor/pc-nrfutil.git`

`cd pc-nrfutil`

`curl -O https://bootstrap.pypa.io/get-pip.py`

`sudo python get-pip.py`

`sudo pip install -r requirements.txt`

`sudo python setup.py install`

`nrfutil version`

How to use it (to include the application code):

`nrfutil dfu genpkg --application path-of-your-app-code-hex-file.hex --application-version 0xffff --dev-revision 0xff --dev-type 0xff --sd-req 0xfffe /Users/lauri/Dropbox/RuuviTag_ApplicationCode.zip`

If want to create distribution package that includes both bootloader and application code:

`nrfutil dfu genpkg --bootloader bootloader/ruuvitag_b1/dual_bank_ble_s132/armgcc/_build/ruuvitag_b1_bootloader.hex --application fw/ruuvitag_b1/s132/armgcc/_build/ruuvitag_b1_fw.hex --application-version 0xffff --dev-revision 0xff --dev-type 0xff --sd-req 0xfffe /Users/lauri/Dropbox/RuuviTag_Bootloader_and_FW.zip`

If you would like to create a package that includes SoftDevice + bootloader, use this command:

`nrfutil dfu genpkg --softdevice nRF5_SDK_11.0.0_89a8197/components/softdevice/s132/hex/s132_nrf52_2.0.0_softdevice.hex --bootloader bootloader/ruuvitag_b2/dual_bank_ble_s132/armgcc/_build/ruuvitag_b2_bootloader.hex --application-version 0xffff --dev-revision 0xff --dev-type 0xff --sd-req 0xfffe /Users/lauri/Dropbox/RuuviTag_SoftDevice_and_Bootloader.zip`

Zip created at /Users/lauri/Dropbox/RuuviTag_SoftDevice_and_bootloader_sdk11.zip

# Compiling

`make` downloads Nordic Semiconductor's nRF52 SDK and extracts it. First time use will probably fail as SDK Makefile defines the path of toolchain, and the default path might differ from your system. 

Modify $SDK/components/toolchain/gcc/Makefile.posix (on Linux and OSX) or Makefile.windows on windows
to point to your gcc-arm install location. 

Second time running `make` builds all the sources, currently B2 and B3 bootloaders and nfc-record-url example. 

`make clean` cleans the build directories.

For more help, please request an invite to Ruuvi Community's Slack channel by emailing us an informal request: slack@ruuvi.com

# Flashing

If the device is empty (no SoftDevice S132 + bootloader flashed), you need to flash using SWD interface. The easiest way is to use nRF52 development kit (PCA10036 or PCA10040) with embedded Segger. Steps:

Download and install latest J-Link https://www.segger.com/jlink-software.html

Start the J-Link from command line by typing:

`JLinkExe -device nrf52 -if swd -speed 1000`

SoftDevice is Nordic Semiconductor's Bluetooth Smart (or ANT) protocol stack. Sources are super secret, but the latest version is always bundled with the SDK. So, let's flash it:

`J-Link>loadfile nRF5_SDK_11.0.0_89a8197/components/softdevice/s132/hex/s132_nrf52_2.0.0_softdevice.hex`

After the SoftDevice is flashed successfully, flash the bootloader:

`J-Link>loadfile bootloader/ruuvitag_b2/dual_bank_ble_s132/armgcc/_build/ruuvitag_b2_bootloader.hex`

After this no cables are needed, ever (unless the device needs to be rescued for some reason)! From now on, the FW (and/or the bootloader and/or the SoftDevice) can be updated Over-The-Air using Nordic's nRF Toolbox (or MasterControl Panel):

https://www.nordicsemi.com/eng/Products/nRFready-Demo-Apps/nRF-Toolbox-App

https://github.com/NordicSemiconductor/Android-nRF-Toolbox

https://github.com/NordicSemiconductor/IOS-nRF-Toolbox

# If you're a developer, this is probably what you're after:

1. Flash the SoftDevice protocol stack
2. Compile the bootloader (and the application)
3. Flash the bootloader
4. Reset the device
5. Create .zip distribution package (that includes at least the application code)
6. Install nRF Toolbox (Android/iOS)
7. Press DFU OTA button
8. After completed, press it again! And again! No more cables needed ^^
9. Now you can update SoftDevice and/or bootloader and/or application using DFU OTA. Cool, huh?
