#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

HTML_PATH = src/main/html
INC_PATH = src/main/include
DATA_TO_H = ./src/data_to_headers.py
HTML_FILES := $(wildcard $(HTML_PATH)/*.html)
HTML_HEADERS := $(patsubst $(HTML_PATH)/%.html,$(INC_PATH)/%.h,$(HTML_FILES))

IMAGE_PATH = src/main/html
IMAGE_FILES := $(wildcard $(IMAGE_PATH)/*.jpg)
IMAGE_HEADERS := $(patsubst $(IMAGE_PATH)/%.jpg,$(INC_PATH)/%.h,$(IMAGE_FILES))

PROJECT_NAME := cat_lamp_server

EXTRA_COMPONENT_DIRS = $(IDF_PATH)/../src

include $(IDF_PATH)/make/project.mk

html: $(HTML_HEADERS)

images: $(IMAGE_HEADERS)

$(INC_PATH)/%.h: $(HTML_PATH)/%.html
	$(DATA_TO_H) $<
	mv $(HTML_PATH)/$(notdir $@) $(INC_PATH)

$(INC_PATH)/%.h: $(HTML_PATH)/%.jpg
	$(DATA_TO_H) $<
	mv $(IMAGE_PATH)/$(notdir $@) $(INC_PATH)

.PHONY: html images