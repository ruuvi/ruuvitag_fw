/* Linker script to configure memory regions. */

SEARCH_DIR(.)
GROUP(-lgcc -lc -lnosys)

MEMORY
{
  FLASH (rx) : ORIGIN = 0x1f000, LENGTH = 0x56000    /* Conserve space for debug bootloader */
  RAM (rwx) :  ORIGIN = 0x20002c38, LENGTH = 0xd3c8  /* <- Configure according to central/peripheral link count an service count */
    /** Location of bootloader setting in flash. */
  BOOTLOADER_SETTINGS (rw) : ORIGIN = 0x0007F000, LENGTH = 0x1000
}

SECTIONS
{
  .fs_data :
  {
    PROVIDE(__start_fs_data = .);
    KEEP(*(.fs_data))
    PROVIDE(__stop_fs_data = .);
  } > RAM
  .pwr_mgmt_data :
  {
    PROVIDE(__start_pwr_mgmt_data = .);
    KEEP(*(.pwr_mgmt_data))
    PROVIDE(__stop_pwr_mgmt_data = .);
  } > RAM
  /* Place the bootloader settings page in flash. */
  .bootloaderSettings(NOLOAD) :
  {

  } > BOOTLOADER_SETTINGS
} INSERT AFTER .data;

INCLUDE "nrf5x_common.ld"
