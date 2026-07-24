//
// Created by msol0v on 24.07.2026.
//

#ifndef SMARTCAMERA_ROUTER_H
#define SMARTCAMERA_ROUTER_H

#include <stdint.h>



typedef enum
{
    HTTP_GET,
    HTTP_POST
} HttpMethod_t;

typedef struct {
    HttpMethod_t method;
    char uri[64];
    char body[512];
} HttpRequest_t;

typedef struct{
    uint16_t status;
    char contentType[32];
    char body[1024];
} HttpResponse_t;

void handleRequest(HttpRequest_t *request,
                        HttpResponse_t *response);

#endif //SMARTCAMERA_ROUTER_H