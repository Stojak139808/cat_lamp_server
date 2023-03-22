#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

HTML_PATH = src/main/html
INC_PATH = src/main/include
HTML_TO_H = ./src/html_to_c.py
HTML_FILES := $(wildcard $(HTML_PATH)/*.html)
HTML_HEADERS := $(patsubst $(HTML_PATH)/%.html,$(INC_PATH)/%.h,$(HTML_FILES))

PROJECT_NAME := cat_lamp_server

EXTRA_COMPONENT_DIRS = $(IDF_PATH)/../src
EXTRA_COMPONENT_DIRS += $(IDF_PATH)/examples/common_components

include $(IDF_PATH)/make/project.mk

html: $(HTML_HEADERS)

$(INC_PATH)/%.h: $(HTML_PATH)/%.html
	$(HTML_TO_H) $<
	mv $(HTML_PATH)/$(notdir $@) $(INC_PATH)

.PHONY: html