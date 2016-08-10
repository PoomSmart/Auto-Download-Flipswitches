DEBUG = 0
TARGET = iphone:latest
PACKAGE_VERSION = 0.0.1

include $(THEOS)/makefiles/common.mk

AGGREGATE_NAME = AutoDownloadSwitchs
SUBPROJECTS = Apps Updates Music Books

include $(THEOS_MAKE_PATH)/aggregate.mk
