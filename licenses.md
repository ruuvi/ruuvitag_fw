# Licenses 

## Nordic SDK
Nordic SDK provides interface to nRF52 chip and various libraries for software development. [Nordic SDK license](<licenses/Noric SDK Licesnse.md>) applies to nRF5_SDK folders, with the exception of `external` subfolder. It also applies to `bsp` folder, `ruuvi_examples` folder and `sdk_overrides` folder.

## Softdevice s132 V3.1.0
Softdevice is a software radio stack which is used to implement BLE communication on top of nRF52832 integrated circuit. It is distributed as a binary only, embedded within nRF52832.
License is Nordic softdevice license agreement. Full text is available in Nordic SDK and [licenses folder](<licenses/<Noridc Softdevice license.md>)

Softdevice available at https://www.nordicsemi.com/eng/nordic/Products/nRF52832/S132-SD-v3/56261 .

## Bootloader
Bootloader is built on top of Nordic example sources and [nRF5 SDK License](<licenses/Noric SDK Licesnse.md>) applies to it. Additionally the bootloader uses micro-ecc library, available at https://github.com/kmackay/micro-ecc . Micro-ECC is released under [BSD-2 license](<licenses/micro-ecc.md>), copyright 2014 Kenneth MacKay.

Bootloader licensing applies to `bootloader` folder. 
Bootloader is statically linked against Softdevice S132 3.1.0

## Ruuvi firmware
Ruuvi firmware is built by Ruuvi community and it is BSD-3 licensed, Nordic SDK licensed or license-free (no explicit license).
These licenses apply to folders `drivers`, `keys`, `libraries`, and `licenses`. 
Files which are under Nordic SDK license can be identified by a comment in the header. 
Copyleft or BSD-3 status can be identified from the list below.

Copyright in alphabetical order of GitHub usernames, real name and email are listed for user who have listed it on their profile:

angst7: Matt Roche <angst7@gmail.com> (license-free)
dieweltistklein: (license-free)
DG12:   Dennis German <DGermang@Real-World-Systems.com> (BSD-3)
frawau: François Wautier (BSD-3)
jessenic: (license-free)
jrosberg: Janne Rosberg (BSD-3)
laurijamsa: Lauri Jämsä <lauri@ruuvi.com> (BSD-3)
mdxs: (license-free)
ojousima: Otso Jousimaa <otso@ruuvi.com> (BSD-3)
scrin: (license-free)

Ruuvi firmware is statically linked against bootloader and softdevice. 
