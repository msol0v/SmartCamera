//
// Created by msol0v on 24.07.2026.
//

#include "router.h"

#include <stdio.h>
#include "lwip/apps/httpd.h"
#include "api.h"

#include <string.h>

#include "fs.h"

static void http_write_json(struct fs_file *file, const char *json){
    const int json_len = strlen(json);
    static char response[512];

    file->len = snprintf(response, sizeof(response),
        "HTTP/1.1 200 OK\r\n"
        "Server: SmartCamera\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %d\r\n"
        "Cache-Control: no-cache\r\n"
        "Connection: close\r\n"
        "\r\n"
        "%s",
        json_len,
        json);

    file->data = response;
    file->index = 0;
    file->flags = FS_FILE_FLAGS_HEADER_INCLUDED;
    file->is_custom_file = 1;
}

//  У товарищей из ST ошибка, если не выставить LWIP_HTTPD_DYNAMIC_FILE_READ, то будет assert подниматься
// на работу вроде не влияет но бесит
int fs_read_custom(struct fs_file *file, char *buffer, int count)
{
    int bytes_left = file->len - file->index;

    if (bytes_left <= 0) {
        return FS_READ_EOF;
    }

    if (count > bytes_left) {
        count = bytes_left;
    }

    memcpy(buffer, file->data + file->index, count);
    file->index += count;

    return count;
}


/*Выставлен CustomFs. Будет вызываться этот колбек
 * для запросов GET
 */
int fs_open_custom(struct fs_file *file, const char *name)
{

    char json[128];


    printf("%s\r\n", name);

    if(strcmp(name, "/api/state") == 0)
    {
        API_GET_State(json);
        http_write_json(file, json);
        return 1;
    }

    if(strcmp(name,"/api/mode")==0)
    {
        //API_GET_Mode(json);
        //http_write_json(file, json);
        //return 1;
    }

    return 0;
}

void fs_close_custom(struct fs_file *file) {
    // Закрывать нечего, просто заглушка
}

/* Колбек для запросов POST
 */
err_t httpd_post_begin(void *connection, const char *uri, const char *http_request,
                       u16_t http_request_len, int content_len, char *response_uri,
                       u16_t response_uri_len, u8_t *post_auto_wnd)
{
    LWIP_UNUSED_ARG(connection);
    LWIP_UNUSED_ARG(http_request);
    LWIP_UNUSED_ARG(http_request_len);
    LWIP_UNUSED_ARG(content_len);
    LWIP_UNUSED_ARG(response_uri);
    LWIP_UNUSED_ARG(response_uri_len);

    *post_auto_wnd = 1;

    printf("POST BEGIN: %s\r\n", uri);

    return ERR_OK;
}

err_t httpd_post_receive_data(void *connection,
                              struct pbuf *p)
{
    LWIP_UNUSED_ARG(connection);

    printf("POST DATA (%d bytes)\r\n", p->tot_len);

    return ERR_OK;
}

void httpd_post_finished(void *connection,
                         char *response_uri,
                         uint16_t response_uri_len)
{
    LWIP_UNUSED_ARG(connection);
    LWIP_UNUSED_ARG(response_uri);
    LWIP_UNUSED_ARG(response_uri_len);

    printf("POST FINISHED\r\n");
}
