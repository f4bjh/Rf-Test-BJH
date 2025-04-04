#include <string.h>

#include <freertos/FreeRTOS.h>
#include <esp_http_server.h>
#include <freertos/task.h>
#include <esp_ota_ops.h>
#include <sys/param.h>
#include <esp_http_server.h>
#include "lwip/sockets.h"
#include "esp_log.h"
#include "keep_alive.h"
#include "esp_task_wdt.h"
#include <nvs_flash.h>
#include "esp_check.h"

#include "meas_mgt.h"
#include "keep_alive.h"
#include "wifi.h"

#define EXAMPLE_HTTP_QUERY_KEY_MAX_LEN  (64)
#define tskHTTP_SERVER tskFSM_MEASURMENT+1
#define HTTP_SERVER_WAKE_UP_TICK 250
#define DISABLE_WDT_TASK

/* Type of Unescape algorithms to be used */
#define NGX_UNESCAPE_URI          (1)
#define NGX_UNESCAPE_REDIRECT     (2)

typedef struct {
    SemaphoreHandle_t mutex;
    instance_meas_t  *instance_meas;
    wss_keep_alive_t keep_alive;
} server_ctx_t;

struct async_resp_arg {
    httpd_handle_t hd;
    int fd;
    instance_meas_t *instance_meas;
};

esp_err_t http_server_init(void);
void example_uri_decode(char *dest, const char *src, size_t len);
void ws_async_send(void *arg);
esp_err_t ws_open_fd(httpd_handle_t hd, int sockfd);
void ws_close_fd(httpd_handle_t hd, int sockfd);