idf_component_register(
	SRCS "main.c"
	SRCS "../wifi/softap.c"
	SRCS "../http_server/keep_alive.c"
	SRCS "../http_server/server.c"
	SRCS "../cJSON/cJSON.c"
	INCLUDE_DIRS "../include"
	EMBED_TXTFILES ../http_server/pages/about.html
	EMBED_TXTFILES ../http_server/pages/frequencymeter.html
	EMBED_TXTFILES ../http_server/pages/generator.html
	EMBED_TXTFILES ../http_server/pages/index.html
	EMBED_TXTFILES ../http_server/pages/jquery.gauge.css
	EMBED_TXTFILES ../http_server/pages/jquery.gauge.js
	EMBED_TXTFILES ../http_server/pages/jquery.gauge.min.js
	EMBED_TXTFILES ../http_server/pages/jquery.min.js
	EMBED_TXTFILES ../http_server/pages/powermeter.html
	EMBED_TXTFILES ../http_server/pages/script.js
	EMBED_TXTFILES ../http_server/pages/style.css
	EMBED_TXTFILES ../http_server/pages/upload.html
	EMBED_TXTFILES ../http_server/pages/wifi.html
)
