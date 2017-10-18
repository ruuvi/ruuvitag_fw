#ifndef EDDYSTONE_H
#define EDDYSTONE_H

/**
 *  @brief Helper for advertising Eddystone URLs. 
 *
 *  @param advdata Advertisement data which will be filled with Eddystone URL
 *  @param url url to advertise. May include prefix and suffix bytes, such as 0x03 for https://
 *  @param length length of URL to advertise. 
 *  @return Error code, 0 on success
 */
 ret_code_t eddystone_prepare_url_advertisement(ble_advdata_t* advdata, char* url, size_t length)

#endif
