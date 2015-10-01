/* Linker script for 580 */

#include "da14580_stack_config.h"
#include "da14580_config.h"
#include "da14580_scatter_config.h"

/* some known symbols */
INCLUDE rom.symbols


/* Linker script to configure memory regions.
 * Need modifying for a specific board.
 *   FLASH.ORIGIN: starting address of flash
 *   FLASH.LENGTH: length of flash
 *   RAM.ORIGIN: starting address of RAM bank 0
 *   RAM.LENGTH: length of RAM bank 0
 */
MEMORY
{
  OTP            (r)   : ORIGIN = 0x00040000, LENGTH = 0x8000 /* 32K */
  /* Retention RAM */
#if (BLE_CONNECTION_MAX_USER > 4)
  RETENTION_RAM1 (rw)  : ORIGIN = 0x000800e8, LENGTH = 0x016C
#endif
  /* 0x00080768 is the last address used by the ROM code */
#if (USE_MEMORY_MAP != EXT_SLEEP_SETUP)
  RETENTION_RAM0 (rw)  : ORIGIN = 0x00080768, LENGTH = RETRAM_LEN
  RETENTION_RAM2 (rw)  : ORIGIN = 0x00080768 + RETRAM_LEN, LENGTH = ExtRETRAM_LEN
#else
  RETENTION_RAM2 (rw)  : ORIGIN = 0x00080768, LENGTH = 0x2968
#endif
  /* RAM is divided into regions as below
  RAM            (rwx) : ORIGIN = 0x20000000, LENGTH = 0xA800 /* 42K */
  RAM_IROM1      (rwx) : ORIGIN = 0x20000000, LENGTH = 0x0160
  RAM_IROM2      (rwx) : ORIGIN = 0x20000160, LENGTH = 0x0160
  RAM_IROM3      (rwx) : ORIGIN = 0x200002C0, LENGTH = 0x0080
  RAM_IROM4      (rwx) : ORIGIN = 0x20000340, LENGTH = 0x0100
  RAM_IROM5      (rwx) : ORIGIN = 0x20000440, LENGTH = 0x7AC0
  /*********************************************************************************************
   * END OF OTP - ANYTHING BELOW THIS POINT IS NOT WRITTEN WHEN THE CODE IS BURNED TO THE OTP! *
   *********************************************************************************************/
#if (ZI_AT_7F00 > 0)
  RAM_RW_IRAM50  (rwx) : ORIGIN = 0x20007F00, LENGTH = ZI_AT_7F00
#endif
  /* not retained HEAP */
  RAM_RW_IRAM51  (rw)  : ORIGIN = (0x20009000 - NON_RET_HEAP_SIZE), LENGTH = NON_RET_HEAP_SIZE
  /* reserved for ROM code */
  RAM_RW_IRAM52  (r)   : ORIGIN = 0x20009000, LENGTH = 0x0020
  RAM_RW_IRAM53  (rw)  : ORIGIN = 0x20009020, LENGTH = 0x01E0
  RAM_RW_IRAM54  (rw)  : ORIGIN = 0x20009200, LENGTH = 0x0600
}

/* Linker script to place sections and symbol values. Should be used together
 * with other linker script that defines memory regions FLASH and RAM.
 * It references following symbols, which must be defined in code:
 *   Reset_Handler : Entry of reset handler
 *
 * It defines following symbols, which code can use without definition:
 *   __exidx_start
 *   __exidx_end
 *   __copy_table_start__
 *   __copy_table_end__
 *   __zero_table_start__
 *   __zero_table_end__
 *   __etext
 *   __data_start__
 *   __preinit_array_start
 *   __preinit_array_end
 *   __init_array_start
 *   __init_array_end
 *   __fini_array_start
 *   __fini_array_end
 *   __data_end__
 *   __bss_start__
 *   __bss_end__
 *   __end__
 *   end
 *   __HeapLimit
 *   __StackLimit
 *   __StackTop
 *   __stack
 */
ENTRY(Reset_Handler)

SECTIONS
{
	ER_IROM1 0x20000000 :
	{
		KEEP(*(.isr_vector))
		. = 0x160 ;
	} > RAM_IROM1 = 0x00000000

	ER_IROM2 0x20000160 :
	{
		KEEP(*(jump_table_mem_area))
	} > RAM_IROM2

	ER_IROM3 0x200002C0 :
	{
		KEEP(*(timeout_table_area))
	} > RAM_IROM3

	ER_IROM4 0x20000340 :
	{
		KEEP(*(nvds_data_storage_area))
	} > RAM_IROM4

	ER_IROM5 0x20000440 :
	{
		*(.text*)

		KEEP(*(.init))
		KEEP(*(.fini))

		/* .ctors */
		*crtbegin.o(.ctors)
		*crtbegin?.o(.ctors)
		*(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
		*(SORT(.ctors.*))
		*(.ctors)

		/* .dtors */
 		*crtbegin.o(.dtors)
 		*crtbegin?.o(.dtors)
 		*(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
 		*(SORT(.dtors.*))
 		*(.dtors)

		*(.rodata*)

		*(.constdata*)

		KEEP(*(.eh_frame*))

#if (SPOTAR_PATCH_AREA == 1)
		*app_spotar.o(spotar_patch_area)
#endif
	} > RAM_IROM5

#if 0
	/* To copy multiple ROM to RAM sections,
	 * uncomment .copy.table section and,
	 * define __STARTUP_COPY_MULTIPLE in startup_ARMCMx.S */
	.copy.table :
	{
		. = ALIGN(4);
		__copy_table_start__ = .;
		LONG (__etext)
		LONG (__data_start__)
		LONG (__data_end__ - __data_start__)
		LONG (__etext2)
		LONG (__data2_start__)
		LONG (__data2_end__ - __data2_start__)
		__copy_table_end__ = .;
	} > RAM_IROM5
#endif

	/* To clear multiple BSS sections,
	 * uncomment .zero.table section and,
	 * define __STARTUP_CLEAR_BSS_MULTIPLE in startup_ARMCMx.S */
	.zero.table :
	{
		. = ALIGN(4);
		__zero_table_start__ = .;
		LONG (__bss_start__)
		LONG (__bss_end__ - __bss_start__)
		LONG (__bss2_start__)
		LONG (__bss2_end__ - __bss2_start__)
		LONG (__bss3_start__)
		LONG (__bss3_end__ - __bss3_start__)
		__zero_table_end__ = .;
	} > RAM_IROM5

	__etext = .;

	.data : AT (__etext)
	{
		__data_start__ = .;
		*(vtable)
		*(.data*)
		__data_end__ = .;

	} > RAM_IROM5

#if (ZI_AT_7F00 > 0)
	RW_IRAM50 0x20007F00 :
	{
#  if (USE_MEMORY_MAP == EXT_SLEEP_SETUP)
		*(retention_mem_area0)  /* application data */
#  endif
		. = ALIGN(4);
		__bss2_start__ = .;
		*(COMMON)
		. = ALIGN(4);
		__bss2_end__ = .;

		/* . = ZI_AT_7F00 ; */
	} > RAM_RW_IRAM50
#endif

	RW_IRAM51 (0x20009000 - NON_RET_HEAP_SIZE) (NOLOAD) :
	{
		*(heap_mem_area_not_ret)
	} > RAM_RW_IRAM51

	/* reserved for ROM code */
	RW_IRAM52 0x20009000 :
	{
		. = 0x20 ;
	} > RAM_RW_IRAM52 = 0x00000000

	.bss :
	{
		. = ALIGN(4);
		__bss_start__ = .;
		*(.bss*)
		. = ALIGN(4);
		__bss_end__ = .;
	} > RAM_RW_IRAM53

#if (ZI_AT_7F00 <= 0)
	.bss2 :
	{
		. = ALIGN(4);
		__bss2_start__ = .;
		*(COMMON)
		. = ALIGN(4);
		__bss2_end__ = .;
	} > RAM_RW_IRAM53
#endif

	.heap (COPY):
	{
		__end__ = .;
		PROVIDE(end = .);
		*(.heap*)
		__HeapLimit = .;
	} > RAM_RW_IRAM53

	/* .stack_dummy section doesn't contain any symbols. It is only
	 * used for linker to calculate size of stack sections, and assign
	 * values to stack symbols later */
	.stack_dummy (COPY):
	{
		*(.stack*)
	} > RAM_RW_IRAM54

	/* Set stack top to end of RAM, and stack limit move down by
	 * size of stack_dummy section */
	__StackTop = ORIGIN(RAM_RW_IRAM54) + LENGTH(RAM_RW_IRAM54);
	__StackLimit = __StackTop - SIZEOF(.stack_dummy);
	PROVIDE(__stack = __StackTop);

#if (BLE_CONNECTION_MAX_USER > 4)
	ZI_RET1 0x00800e8 :
	{
#  if (USE_MEMORY_MAP != EXT_SLEEP_SETUP)
		*(retention_mem_area1)
#  else
		*(retention_mem_area0)
#  endif
	} > RETENTION_RAM1
#endif  /* BLE_CONNECTION_MAX_USER > 4 */

#if (USE_MEMORY_MAP != EXT_SLEEP_SETUP)

	ZI_RET00 0x00080768 (NOLOAD) :
	{
#  if (BLE_CONNECTION_MAX_USER > 4)
		arch_main.o(cs_area)
#  endif
		*(retention_mem_area0)  /* application data */
		*(heap_env_area)
		*(heap_db_area)
		*(heap_msg_area)
		/* SPOTAR */
#  if (SPOTAR_PATCH_AREA == 0)
		/* Placed in the RetRAM when SPOTAR_PATCH_SYSRAM is 0 */
		*app_spotar.o(spotar_patch_area)
#  endif
	} > RETENTION_RAM0

	ZI_RET20 0x00080768 + RETRAM_LEN (NOLOAD) :
	{
		__ZI_RET20_start = ABSOLUTE(.);
		. = 0x00081A20 - __ZI_RET20_start;
		*(BLE_exchange_memory)
		__bss3_start__ = .;
		*(.bss*)
		. = ALIGN(4);
		__bss3_end__ = .;
	} > RETENTION_RAM2

#else  /* USE_MEMORY_MAP == EXT_SLEEP_SETUP */

	ZI_RET20 0x00080768 (NOLOAD) :
	{
#  if (BLE_CONNECTION_MAX_USER > 4)
		*arch_main.o(cs_area)
#  endif
		*(heap_env_area)
		*(heap_db_area)
		*(heap_msg_area)
		*(retention_mem_area0)  /* application data */
	} > RETENTION_RAM2

	ZI_RET21 ADDR(ZI_RET20) + SIZEOF(ZI_RET20) (NOLOAD) :
	{
		__bss3_start__ = .;
		__ZI_RET21_start = ABSOLUTE(.);
		*(.bss*)
		. = ALIGN(4);
		__bss3_end__ = .;
		. = 0x00082A20 - __ZI_RET21_start;
		*(BLE_exchange_memory)
	} > RETENTION_RAM2

#endif  /* USE_MEMORY_MAP == EXT_SLEEP_SETUP */

	/DISCARD/ :
	{
#if 0
		hardfault_handler.o(.text)
		gpio.o(i.GPIO_SetPinFunction)
		gpio.o(i.GPIO_SetInactive)
		gpio.o(i.GPIO_SetActive)
		gpio.o(i.GPIO_ConfigurePin)
		gpio.o(i.GPIO_GetPinStatus)
		atts_task.obj(i.__ARM_common_switch8)
#endif
		*(.ARM.extab* .gnu.linkonce.armextab.*)
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
	}

	/* Check if data + heap + stack exceeds RAM limit */
	ASSERT(__StackLimit >= __HeapLimit, "region RAM overflowed with stack")
}
