UNAME := $(shell uname -s)
COMPILE := echo "OS Not supported!"
CLEAN := echo "OS Not supported!"

ifeq "$(UNAME)" "Linux"
COMPILE := gcc -o Linux/bin/OneBootlogoTools OneBootlogoTools/main.c -DLINUX
CLEAN := rm -rf Linux/bin
endif
ifeq "$(UNAME)" "Darwin"
COMPILE := xcodebuild
CLEAN := xcodebuild clean
endif

MKDIR := $(shell mkdir -p Linux/bin/)

all:
	$(MKDIR)
	$(COMPILE)

clean:
	$(CLEAN)
