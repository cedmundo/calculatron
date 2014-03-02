# Neocognitron Structure.
# License: See README.TXT

NEO_PATH = neocognitron/

LOCAL_LDLIBS 	+= -lm
LOCAL_SRC_FILES += \
	$(NEO_PATH)/MemoryManage.cc \
	$(NEO_PATH)/Makers.cc \
	$(NEO_PATH)/Layers.cc \
	$(NEO_PATH)/SelfOrganitation.cc \
	$(NEO_PATH)/InputOutput.cc \
	$(NEO_PATH)/Neocognitron.cc \