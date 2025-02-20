#define EXAMPLE_HTTP_QUERY_KEY_MAX_LEN  (64)

/* Type of Unescape algorithms to be used */
#define NGX_UNESCAPE_URI          (1)
#define NGX_UNESCAPE_REDIRECT     (2)

esp_err_t http_server_init(void);

