idf_component_register(
	SRCS "main.c"
	SRCS "../http_server/keep_alive.c"
	SRCS "../http_server/server.c"
	SRCS "../cJSON/cJSON.c"
	SRCS "../data/chip_info.c"
	SRCS "../data/data.c"
	SRCS "../lcd/lcd.c"
	SRCS "../lcd/lvgl.c"
	SRCS "../wifi/softap_sta.c"
	INCLUDE_DIRS "../include"
	EMBED_FILES ../http_server/pages/about.html
	EMBED_FILES ../http_server/pages/frequencymeter.html
	EMBED_FILES ../http_server/pages/generator.html
	EMBED_FILES ../http_server/pages/index.html
	EMBED_FILES ../http_server/pages/jquery.gauge.css
	EMBED_FILES ../http_server/pages/jquery.gauge.js
	EMBED_FILES ../http_server/pages/jquery.gauge.min.js
	EMBED_FILES ../http_server/pages/jquery.min.js
	EMBED_FILES ../http_server/pages/powermeter.html
	EMBED_FILES ../http_server/pages/script.js
	EMBED_FILES ../http_server/pages/style.css
	EMBED_FILES ../http_server/pages/upload.html
	EMBED_FILES ../http_server/pages/wifi.html
)
