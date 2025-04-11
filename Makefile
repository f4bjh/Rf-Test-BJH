PROJECT_NAME := Rf-Test-BJH

IDF_PY := idf.py

FACTORY_CONFIG := sdkconfig.defaults.factory
OTA_CONFIG := sdkconfig.defaults.ota

PORT ?= /dev/ttyACM0
BAUD ?= 460800

# Récupérer les adresses de partition depuis partitions.csv
PARTITION_FILE := partitions.csv

# Fonction pour extraire l'adresse d'une partition donnée depuis partitions.csv
define extract_partition_address
	$(shell awk -F, '$$1 == "$(1)" {print $$4}' $(PARTITION_FILE))
endef

# Extraire les adresses pour les partitions
ADDR_FACTORY := $(call extract_partition_address,factory)
ADDR_OTA := $(call extract_partition_address,ota_0)
ADDR_BOOTLOADER := 0x1000
ADDR_PARTITION_TABLE := 0x8000

.PHONY: all factory ota clean_flash clean_factory clean_ota flash_factory help

# Concaténer sdkconfig.defaults.common et sdkconfig.factory (ou sdkconfig.ota)
define concat_sdkconfig
	@cat sdkconfig.defaults.common $(1) > sdkconfig
endef

all: factory ota 

# Build du firmware factory
factory:
	@echo "🛠️  Build firmware FACTORY"
	$(call concat_sdkconfig, $(FACTORY_CONFIG))
	$(IDF_PY) fullclean
	$(IDF_PY) build
	cp build/$(PROJECT_NAME).bin build/$(PROJECT_NAME)_factory.bin

# Build du firmware OTA
ota:
	@echo "🛠️  Build firmware OTA"
	$(call concat_sdkconfig, $(OTA_CONFIG))
	$(IDF_PY) fullclean
	$(IDF_PY) build
	cp build/$(PROJECT_NAME).bin build/$(PROJECT_NAME)_ota.bin


# Flash du firmware factory
flash_factory: build/$(PROJECT_NAME)_factory.bin 
	@echo "🚀 Flash firmware FACTORY @ $(ADDR_FACTORY)"
	esptool.py --chip esp32s3 -p /dev/ttyACM0 -b 460800 --before=default_reset --after=hard_reset write_flash --flash_mode dio --flash_freq 80m --flash_size 16MB 0x0 build/bootloader/bootloader.bin 0x10000 build/$(PROJECT_NAME)_factory.bin 0x8000 build/partition_table/partition-table.bin 0xd000 build/ota_data_initial.bin

# Commande de nettoyage pour la version factory
clean_factory:
	$(IDF_PY) fullclean
	rm -f build/$(PROJECT_NAME)_factory.bin

# Commande de nettoyage pour la version OTA
clean_ota:
	$(IDF_PY) fullclean
	rm -f build/$(PROJECT_NAME)_ota.bin

# Affichage de l'aide pour chaque commande
help:
	@echo "-------------------- Liste des commandes --------------------"
	@echo "make all                : Build et flash les firmwares (factory et ota)"
	@echo "make factory            : Build uniquement le firmware factory"
	@echo "make ota                : Build uniquement le firmware OTA"
	@echo "make flash_factory      : Flash le firmware factory sur la partition appropriée"
	@echo "make clean_factory      : Supprime les fichiers de build et firmware de la version factory"
	@echo "make clean_ota          : Supprime les fichiers de build et firmware de la version OTA"
	@echo "make help               : Affiche cette aide"
	@echo "------------------------------------------------------------"

