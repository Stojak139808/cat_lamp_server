#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := cat_lamp_server

EXTRA_COMPONENT_DIRS = $(IDF_PATH)/../src
EXTRA_COMPONENT_DIRS += $(IDF_PATH)/examples/common_components

include $(IDF_PATH)/make/project.mk

