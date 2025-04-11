# Inclure les fichiers sources conditionnellement
if(CONFIG_FIRMWARE_FACTORY)
    list(APPEND SRCS
        "main.c"
	"../http_server/keep_alive.c"
	"../http_server/server.c"
	"../http_server/pages/upload.c"
	"../lcd/lcd.c"
	"../lcd/lvgl.c"
	"../wifi/softap_sta.c"
    ) 
    list(APPEND EMBED_FILES 
	EMBED_FILES ../http_server/pages/factory.html
	EMBED_FILES ../http_server/pages/style.css
    )	
endif()

if(CONFIG_FIRMWARE_OTA)
	list(APPEND SRCS
        "main.c"
	"../http_server/keep_alive.c"
	"../http_server/server.c"
	"../http_server/ws.c"
	"../http_server/pages/index.c"
	"../http_server/pages/about.c"
	"../http_server/pages/wifi.c"
	"../http_server/pages/frequencymeter.c"
	"../http_server/pages/generator.c"
	"../http_server/pages/powermeter.c"
	"../http_server/pages/upload.c"
	"../cJSON/cJSON.c"
	"../meas_mgt/meas_mgt.c"
	"../meas_mgt/fsm.c"
	"../lcd/lcd.c"
	"../lcd/lvgl.c"
	"../wifi/softap_sta.c"
	"../meas/counter/counter.c"
	"../meas/chip_info/chip_info.c"
	"../meas/part_info/part_info.c"
	"../meas/freq/freq.c"
    )
    list(APPEND EMBED_FILES 
	../http_server/pages/about.html
	../http_server/pages/frequencymeter.html
	../http_server/pages/frequencymeter.js	
	../http_server/pages/generator.html
	../http_server/pages/index.html
	../http_server/pages/index.js
	../http_server/pages/upload.html
	../http_server/pages/upload.js
	../http_server/pages/wifi.html
	../http_server/pages/jquery.gauge.css
	../http_server/pages/jquery.gauge.js
	../http_server/pages/jquery.gauge.min.js
	../http_server/pages/jquery.min.js
	../http_server/pages/powermeter.html
	../http_server/pages/style.css
    )
endif()


# Enregistrez le composant avec les fichiers sources conditionnels
idf_component_register(
   SRCS ${SRCS}
   INCLUDE_DIRS "../include"
   EMBED_FILES ${EMBED_FILES}
)
