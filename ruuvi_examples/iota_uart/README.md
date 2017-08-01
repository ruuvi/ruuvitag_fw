# IOTA UART example
PoC program, creates a fixed MAM message and parses it back into cleartext.
Results are printed to UART on PCA10056 (nRF52840 dev kit). 

In future the Uart communication will be portedto BLE using Nordic Uart Service.

# Boards
Currently program runs only on PCA10056.
RuuviTag is WIP. Before sensors are used, it's recommended to 
use PCA10040 (nRF52832 dev kit) as a target. 

# Compiling
To compile program you need:
 * nRF52 SDK 12.3
   - Run "make" at repository root to download and unzip SDK
 * iota.lib.c
   - Added as a submodule
   - run git submodule update --init --recursive at top level
 * MAM
   - Compiled library added, for sources please see (github)[https://github.com/iotaledger/MAM].

Set up the toolchain as instructed in top level readme of this repository
Run "make" in pca10056/s132/armgcc

# Flashing
nRF52 code is generally split in 3 parts: softdevice (radio), bootloader and application. This example uses only softdevice and application
 * Install Nordic Commad Line tools
 * Softdevice is s132 3.x.0, 3.0.0 can be found in SDK/components/softdevice, 3.1.0 at Nordic website.
 * Application is in _build directory of the project, i.e. pca10056/s132/armgcc/build/nrf52840_xxaa.hex
 * run `nrfjprog --family nrf52 --eraseall` `nrfjprog --family nrf52 --program $SOFTDEVICE` `nrfjprog --family nrf52 --program $APPLICATION` `nrfjprog --family nrf52 --reset`
 * Application can be updated by `nrfjprog --family nrf52 --sectoranduicrerase --program $APPLICATION`

# Licensing
  * nRF5 SDK is under Nordic License, in effect the code of SDK can be used only on Nordic chips
  * All Ruuvi contributions are BSD-3
  * iota.lib.c is under MIT license
  * Please contact IOTA and repository owners for licenses of MAM
  


