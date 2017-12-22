#ifndef SDK_BOARD_CONFIG_H
#define SDK_BOARD_CONFIG_H

// Define any board-specific SDK configuration overrides here

// <h> nRF_NFC 

//==========================================================
// <e> NFC_HAL_ENABLED - nfc_t2t_hal - Hardware Abstraction Layer for NFC library.
//==========================================================
#ifndef NFC_HAL_ENABLED
#define NFC_HAL_ENABLED 1
#endif
#if  NFC_HAL_ENABLED
// <e> HAL_NFC_CONFIG_LOG_ENABLED - Enables logging in the module.
//==========================================================
#ifndef HAL_NFC_CONFIG_LOG_ENABLED
#define HAL_NFC_CONFIG_LOG_ENABLED 0
#endif
#if  HAL_NFC_CONFIG_LOG_ENABLED
// <o> HAL_NFC_CONFIG_LOG_LEVEL  - Default Severity level
 
// <0=> Off 
// <1=> Error 
// <2=> Warning 
// <3=> Info 
// <4=> Debug 

#ifndef HAL_NFC_CONFIG_LOG_LEVEL
#define HAL_NFC_CONFIG_LOG_LEVEL 3
#endif

// <o> HAL_NFC_CONFIG_INFO_COLOR  - ANSI escape code prefix.
 
// <0=> Default 
// <1=> Black 
// <2=> Red 
// <3=> Green 
// <4=> Yellow 
// <5=> Blue 
// <6=> Magenta 
// <7=> Cyan 
// <8=> White 

#ifndef HAL_NFC_CONFIG_INFO_COLOR
#define HAL_NFC_CONFIG_INFO_COLOR 0
#endif

// <o> HAL_NFC_CONFIG_DEBUG_COLOR  - ANSI escape code prefix.
 
// <0=> Default 
// <1=> Black 
// <2=> Red 
// <3=> Green 
// <4=> Yellow 
// <5=> Blue 
// <6=> Magenta 
// <7=> Cyan 
// <8=> White 

#ifndef HAL_NFC_CONFIG_DEBUG_COLOR
#define HAL_NFC_CONFIG_DEBUG_COLOR 0
#endif

#endif //HAL_NFC_CONFIG_LOG_ENABLED
// </e>

// <e> HAL_NFC_CONFIG_DEBUG_PIN_ENABLED - Enables pin debug in the module.
//==========================================================
#ifndef HAL_NFC_CONFIG_DEBUG_PIN_ENABLED
#define HAL_NFC_CONFIG_DEBUG_PIN_ENABLED 0
#endif
#if  HAL_NFC_CONFIG_DEBUG_PIN_ENABLED
// <o> HAL_NFC_HCLOCK_ON_DEBUG_PIN - Pin number  <0-31> 


#ifndef HAL_NFC_HCLOCK_ON_DEBUG_PIN
#define HAL_NFC_HCLOCK_ON_DEBUG_PIN 11
#endif

// <o> HAL_NFC_HCLOCK_OFF_DEBUG_PIN - Pin number  <0-31> 


#ifndef HAL_NFC_HCLOCK_OFF_DEBUG_PIN
#define HAL_NFC_HCLOCK_OFF_DEBUG_PIN 12
#endif

// <o> HAL_NFC_NFC_EVENT_DEBUG_PIN - Pin number  <0-31> 


#ifndef HAL_NFC_NFC_EVENT_DEBUG_PIN
#define HAL_NFC_NFC_EVENT_DEBUG_PIN 24
#endif

// <o> HAL_NFC_DETECT_EVENT_DEBUG_PIN - Pin number  <0-31> 


#ifndef HAL_NFC_DETECT_EVENT_DEBUG_PIN
#define HAL_NFC_DETECT_EVENT_DEBUG_PIN 25
#endif

// <o> HAL_NFC_TIMER4_EVENT_DEBUG_PIN - Pin number  <0-31> 


#ifndef HAL_NFC_TIMER4_EVENT_DEBUG_PIN
#define HAL_NFC_TIMER4_EVENT_DEBUG_PIN 28
#endif

#endif //HAL_NFC_CONFIG_DEBUG_PIN_ENABLED
// </e>

// <o> HAL_NFC_FIELD_TIMER_PERIOD - TIMER4 period in us (used for workaround)  <100-10000> 


// <i> This value is used as a timeout for polling NFC field to detect FIELDLOST event
// <i> This value can affect Tag behavior

#ifndef HAL_NFC_FIELD_TIMER_PERIOD
#define HAL_NFC_FIELD_TIMER_PERIOD 100
#endif

#endif //NFC_HAL_ENABLED
// </e>

// <e> NFC_NDEF_MSG_ENABLED - nfc_ndef_msg - NFC NDEF Message generator module
//==========================================================
#ifndef NFC_NDEF_MSG_ENABLED
#define NFC_NDEF_MSG_ENABLED 1
#endif
#if  NFC_NDEF_MSG_ENABLED
// <o> NFC_NDEF_MSG_TAG_TYPE  - NFC Tag Type
 
// <2=> Type 2 Tag 
// <4=> Type 4 Tag 

#ifndef NFC_NDEF_MSG_TAG_TYPE
#define NFC_NDEF_MSG_TAG_TYPE 2
#endif

#endif //NFC_NDEF_MSG_ENABLED
// </e>

// </h> 
//==========================================================

#endif
