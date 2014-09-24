UNAME := $(shell uname -s)
COMPILE := echo "OS Not supported!"
CLEAN := echo "OS Not supported!"

ifeq "$(UNAME)" "Linux"
COMPILE := gcc -o Linux/bin/OneBootlogoTools OneBootlogoTools/main.c -D__LINUX__
CLEAN := rm -rf Linux/bin
MKDIR := $(shell mkdir -p Linux/bin/)
endif
ifeq "$(UNAME)" "Darwin"
COMPILE := xcodebuild
CLEAN := xcodebuild clean
endif

all:
	$(MKDIR)
	$(COMPILE)

clean:
	$(CLEAN)
