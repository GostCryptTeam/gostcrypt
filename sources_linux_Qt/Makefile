#------ Command line arguments ------
# DEBUG:		Disable optimizations and enable debugging checks
# DEBUGGER:		Enable debugging information for use by debuggers
# NOASM:		Exclude modules requiring assembler
# NOGUI:		Disable graphical user interface (build console-only application)
# NOSTRIP:		Do not strip release binary
# NOTEST:		Do not test release binary
# RESOURCEDIR:	Run-time resource directory
# VERBOSE:		Enable verbose messages
# WXSTATIC:		Use static wxWidgets library

#------ Targets ------
# all
# clean


#------ Build configuration ------

export APPNAME := gostcrypt
export BASE_DIR := $(CURDIR)
export BUILD_INC := $(BASE_DIR)/Build/Include

export AR ?= ar
export CC ?= gcc
export CXX ?= g++
export AS := nasm
export RANLIB ?= ranlib

export CFLAGS := -Wall 
export CXXFLAGS := -Wall -Wno-unused-parameter -std=c++11
C_CXX_FLAGS := -MMD -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGE_FILES -I$(BASE_DIR) -I$(BASE_DIR)/Crypto
export ASFLAGS := -Ox -D __GNUC__
export LFLAGS :=

export PKG_CONFIG_PATH = /usr/local/lib/pkgconfig
export PKCS11_INC = Pkcs11

export GST_BUILD_CONFIG := Debug

ifeq "$(origin DEBUG)" "command line"
	ifneq "$(DEBUG)" "0"
		GST_BUILD_CONFIG := Debug
	endif
endif

ifeq "$(origin NOGUI)" "command line"
	export GST_NO_GUI := 1
	export APPNAME := gostcrypt.cli
	C_CXX_FLAGS += -DGST_NO_GUI
endif

C_CXX_FLAGS += -I$(PKCS11_INC)

ifeq "$(origin RESOURCEDIR)" "command line"
	C_CXX_FLAGS += -DGST_RESOURCE_DIR="$(RESOURCEDIR)"
endif

ifneq "$(origin VERBOSE)" "command line"
	MAKEFLAGS += -s
endif

#------ Release configuration ------

ifeq "$(GST_BUILD_CONFIG)" "Release"

	C_CXX_FLAGS += -O2 -fno-strict-aliasing  # Do not enable strict aliasing
	export WX_BUILD_DIR ?= $(BASE_DIR)/wxrelease

else

#------ Debug configuration ------

	C_CXX_FLAGS += -DDEBUG -g
	CXXFLAGS += -fno-default-inline -Wno-unused-function -Wno-unused-variable

endif


#------ Debugger configuration ------

ifeq "$(origin DEBUGGER)" "command line"

	C_CXX_FLAGS += -ggdb  

endif


#------ Platform configuration ------

export PLATFORM := "Unknown"
export PLATFORM_UNSUPPORTED := 0

export CPU_ARCH ?= unknown

ARCH = $(shell uname -p)
ifeq "$(ARCH)" "unknown"
	ARCH = $(shell uname -m)
endif

ifneq (,$(filter i386 i486 i586 i686 x86,$(ARCH)))
	CPU_ARCH = x86
	ASM_OBJ_FORMAT = elf32
else ifneq (,$(filter x86_64 x86-64 amd64 x64,$(ARCH)))
	CPU_ARCH = x64
	ASM_OBJ_FORMAT = elf64
endif

ifeq "$(origin NOASM)" "command line"
	CPU_ARCH = unknown
endif

ifeq "$(CPU_ARCH)" "x86"
	C_CXX_FLAGS += -D GST_ARCH_X86
else ifeq "$(CPU_ARCH)" "x64"
	C_CXX_FLAGS += -D GST_ARCH_X64
endif


#------ Linux configuration ------

ifeq "$(shell uname -s)" "Linux"

	PLATFORM := Linux
	C_CXX_FLAGS += -DGST_UNIX -DGST_LINUX

	ifeq "$(GST_BUILD_CONFIG)" "Release"
		C_CXX_FLAGS += -fdata-sections -ffunction-sections
		LFLAGS += -Wl,--gc-sections

		ifneq "$(shell ld --help 2>&1 | grep sysv | wc -l)" "0"
			LFLAGS += -Wl,--hash-style=sysv
		endif

		WXCONFIG_CFLAGS += -fdata-sections -ffunction-sections
		WXCONFIG_CXXFLAGS += -fdata-sections -ffunction-sections
	endif
	ifeq "(GST_BUILD_CONFIG)" "Debug"
		C_CXX_FLAGS += -g -Wall
		CFLAGS += -g
	endif		

endif


#------ Mac OS X configuration ------

ifeq "$(shell uname -s)" "Darwin"

	PLATFORM := MacOSX
	APPNAME := GostCrypt

	GST_OSX_SDK ?= /Developer/SDKs/MacOSX10.4u.sdk
	CC := gcc-4.0
	CXX := g++-4.0

	C_CXX_FLAGS += -DGST_UNIX -DGST_BSD -DGST_MACOSX -mmacosx-version-min=10.4 -isysroot $(GST_OSX_SDK)
	LFLAGS += -mmacosx-version-min=10.4 -Wl,-syslibroot $(GST_OSX_SDK)

	ifeq "$(CPU_ARCH)" "x64"
		CPU_ARCH = x86
	endif

	ASM_OBJ_FORMAT = macho
	ASFLAGS += --prefix _

	ifeq "$(GST_BUILD_CONFIG)" "Release"

		export DISABLE_PRECOMPILED_HEADERS := 1

		S := $(C_CXX_FLAGS)
		C_CXX_FLAGS = $(subst -MMD,,$(S))

		C_CXX_FLAGS += -gfull -arch i386 -arch ppc
		LFLAGS += -Wl,-dead_strip -arch i386 -arch ppc
	endif
endif


#------ FreeBSD configuration ------

ifeq "$(shell uname -s)" "FreeBSD"

	PLATFORM := FreeBSD
	PLATFORM_UNSUPPORTED := 1
	C_CXX_FLAGS += -DGST_UNIX -DGST_BSD -DGST_FREEBSD

endif


#------ Solaris configuration ------

ifeq "$(shell uname -s)" "SunOS"

	PLATFORM := Solaris
	PLATFORM_UNSUPPORTED := 1
	C_CXX_FLAGS += -DGST_UNIX -DGST_SOLARIS
endif


#------ Common configuration ------

CFLAGS := $(C_CXX_FLAGS) $(CFLAGS) $(GST_EXTRA_CFLAGS)
CXXFLAGS := $(C_CXX_FLAGS) $(CXXFLAGS) $(GST_EXTRA_CXXFLAGS)
ASFLAGS += -f $(ASM_OBJ_FORMAT)
LFLAGS := $(LFLAGS) $(GST_EXTRA_LFLAGS)

#------ Project build ------

PROJ_DIRS := Platform Volume Driver/Fuse Core

.PHONY: all clean

all clean:
	@if pwd | grep -q ' '; then echo 'Error: source code is stored in a path containing spaces' >&2; exit 1; fi

	@for DIR in $(PROJ_DIRS); do \
		PROJ=$$(echo $$DIR | cut -d/ -f1); \
		$(MAKE) -C $$DIR -f $$PROJ.make NAME=$$PROJ $(MAKECMDGOALS) || exit $?; \
		export LIBS="$(BASE_DIR)/$$DIR/$$PROJ.a $$LIBS -ldl"; \
	done	
