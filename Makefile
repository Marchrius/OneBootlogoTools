UNAME := $(shell uname -s)
ifeq "$(UNAME)" "Linux"
COMPILE := gcc -o Linux/bin/OneBootlogoTools OneBootlogoTools/main.c
CLEAN := rm -rf Linux/bin
endif
ifeq "$(UNAME)" "Darwin"
COMPILE := xcodebuild
CLEAN := xcodebuild clean
else
COMPILE := echo "OS Not supported!"
CLEAN := echo "OS Not supported!"
endif

MKDIR := $(shell mkdir -p Linux/bin/)

all:
	$(MKDIR)
	$(COMPILE)

clean:
	$(CLEAN)
