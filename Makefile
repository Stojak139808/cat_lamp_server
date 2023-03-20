#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := simple

EXTRA_COMPONENT_DIRS = $(IDF_PATH)/..
EXTRA_COMPONENT_DIRS += $(IDF_PATH)/examples/common_components

include $(IDF_PATH)/make/project.mk

