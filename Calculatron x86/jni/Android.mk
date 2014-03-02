LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

# Configure OpenCV.
OPENCV_INSTALL_MODULES:=on
OPENCV_CAMERA_MODULES:=off
OPENCV_LIB_TYPE:=STATIC
include /home/carlos/Proyectos/Calculatron/OpenCV/OpenCV-2.4.5-android-sdk/sdk/native/jni/OpenCV.mk

# Build application.
LOCAL_MODULE    := Calculatron.x86
LOCAL_LDLIBS 	+= -landroid -llog -ljnigraphics
LOCAL_CFLAGS 	:= -UNDEBUG -std=c++11 -Wall # Use for debug build.
#LOCAL_CFLAGS 	:= -std=c++11 -Wall
LOCAL_SRC_FILES := Calculatron.x86.cc Exceptions.cc

# We want to include build of Neocognitron.
include $(LOCAL_PATH)/neocognitron/Neocognitron.mk
include $(BUILD_SHARED_LIBRARY)
