//
// Created by msol0v on 24.07.2026.
//

#include "api.h"

#include <stdio.h>
#include <string.h>
//#include "httpd_structs.h"

void API_GET_State(char *respJSON)
{
   snprintf(respJSON, 128,
             "{\"status\":%d}", 0);
}

// void API_Mode(char *respJSON)
// {
//
// }