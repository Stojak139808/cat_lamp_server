#ifndef HTTP_H
#define HTTP_H

#include "esp_system.h"

typedef struct{
    char* type;
    size_t size;
    void* data;
}raw_http_get_data;

void init_server(void);

#endif