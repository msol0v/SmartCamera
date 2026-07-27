#include "router.h"
#include <stdio.h>
#include <string.h>
#include "lwip/apps/httpd.h"
#include "api.h"
#include "fs.h"

// Буфер для сборки параметров
static char g_cgi_query_string[256];

// CGI Callback: вызывается LwIP, когда приходит запрос на /api?cmd=...
static const char *CGI_ApiHandler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
    // Восстанавливаем query-строку обратно из массива CGI параметров
    g_cgi_query_string[0] = '\0';
    strcat(g_cgi_query_string, "?");

    for (int i = 0; i < iNumParams; i++) {
        if (i > 0) strcat(g_cgi_query_string, "&");
        strcat(g_cgi_query_string, pcParam[i]);
        if (pcValue[i] && strlen(pcValue[i]) > 0) {
            strcat(g_cgi_query_string, "=");
            strcat(g_cgi_query_string, pcValue[i]);
        }
    }

    // Обрабатываем команду парсером
    API_ProcessCommand(g_cgi_query_string);

    // Возвращаем псевдо-путь, который LwIP сразу же откроет через fs_open_custom
    return "/api/state";
}

// Регистрация обработчика CGI
void Router_Init(void)
{
    static const tCGI cgi_handlers[] = {
        { "/api", CGI_ApiHandler } // Автоматически перехватит все /api?cmd=...
    };

    http_set_cgi_handlers(cgi_handlers, 1);
}

static void http_write_json(struct fs_file *file, const char *json) {
    const int json_len = strlen(json);
    static char response[1024];

    file->len = snprintf(response, 1024,
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

int fs_open_custom(struct fs_file *file, const char *name)
{
    static char json_buffer[1024];

    // Вызывается как для прямого /api/state, так и после возврата из CGI_ApiHandler
    if (strcmp(name, "/api/state") == 0 || strcmp(name, "/api") == 0)
    {
        API_GET_State(json_buffer, sizeof(json_buffer));
        http_write_json(file, json_buffer);
        return 1;
    }

    return 0;
}

void fs_close_custom(struct fs_file *file) {
    // Заглушка
}