#ifndef MAM_APP_H
#define MAM_APP_H
#include "ruuvi_endpoints.h"
void send_environmental_mam(void);
ret_code_t mam_handler(const ruuvi_standard_message_t message);
#endif
