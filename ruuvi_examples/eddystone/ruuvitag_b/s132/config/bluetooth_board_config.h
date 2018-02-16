#ifndef BLUETOOTH_BOARD_CONFIG_H
#define BLUETOOTH_BOARD_CONFIG_H

#define BLE_COMPANY_IDENTIFIER              0x0499                                        //!< Ruuvi
#define APP_CONFIG_CALIBRATED_RANGING_DATA  {-51, -41, -32, -26, -22, -17, -15, -10, -3}  //!< Calibrated TX power at 0 m. See the nRF52 Product Specification for corresponding TX values.

/** Device Information Service initial values **/
#define SERIAL_LENGTH                       9                                             /**< 2 UINT32_t hex-encoded + NULL */
#define INIT_MANUFACTURER                   "Ruuvi Innovations Ltd"
#define INIT_MODEL                          "RuuviTag"
#define INIT_HWREV                          "B"

#endif
