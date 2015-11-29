# ruuvitag_fw nrf52 branch

There are 2 main parts on this repository:

* Bootloader
* FW

Prerequisites (for compiling):

* Download and install GCC https://launchpad.net/gcc-arm-embedded/+download

Extract the GCC tarball. Other destinations are also ok, but this one is used often:
sudo mkdir -p /usr/local && cd /usr/local && sudo tar xjf ~/Downloads/gcc-arm-none-eabi-4_xxxxxxxx.tar.bz2

To add to path:

cat /etc/paths.d/arm-gcc
/usr/local/gcc-arm-none-eabi-4_9-2015q3/bin

== Compiling ==

"make" downloads Nordic Semiconductor's nRF52 SDK and extracts it. After that builds the bootloader and the FW.
"make clean" cleans the build directories.

Request an invite to Ruuvi's Slack channel to join our dev talk!

http://ruuvi.com/blog/ruuvi-slack-com.html
