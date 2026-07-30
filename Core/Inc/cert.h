//
// Created by msol0v on 7/30/26.
//

#ifndef SMARTCAMERA_CERT_H
#define SMARTCAMERA_CERT_H

#include <stddef.h>
const char server_cert[] =
"-----BEGIN CERTIFICATE-----\r\n"
""-----END CERTIFICATE-----\r\n";
\r\n"
"-----END CERTIFICATE-----\r\n";

const char server_key[] =
"-----BEGIN RSA PRIVATE KEY-----\r\n"
"MIIE... (ваш приватный ключ) ...\r\n"
"-----END RSA PRIVATE KEY-----\r\n";

const size_t server_cert_len = sizeof(server_cert);
const size_t server_key_len = sizeof(server_key);
#endif //SMARTCAMERA_CERT_H
