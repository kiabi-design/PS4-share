OO_PS4_TOOLCHAIN ?= /opt/pacbrew/ps4/openorbis

CC      := $(OO_PS4_TOOLCHAIN)/bin/linux/x86_64/clang
LD      := $(OO_PS4_TOOLCHAIN)/bin/linux/x86_64/ld.lld
FSELF   := $(OO_PS4_TOOLCHAIN)/bin/linux/x86_64/create-fself
GP4     := $(OO_PS4_TOOLCHAIN)/bin/linux/x86_64/create-gp4
PKGTOOL := $(OO_PS4_TOOLCHAIN)/bin/linux/x86_64/create-pkg

TARGET  := FastDropPS4
SRC     := source/main.c
ELF     := build/$(TARGET).elf
OELF    := build/$(TARGET).oelf
BIN     := pkg_dir/eboot.bin
GP4_XML := FastDropPS4.gp4
OUT_PKG := FastDropPS4_stub.pkg

CFLAGS  := -target x86_64-scei-freebsd \
           -isysroot $(OO_PS4_TOOLCHAIN) \
           -isystem $(OO_PS4_TOOLCHAIN)/include \
           -I$(OO_PS4_TOOLCHAIN)/include/orbis \
           -O2 -Wall -Wextra

LDFLAGS := -target x86_64-scei-freebsd \
           --sysroot=$(OO_PS4_TOOLCHAIN) \
           -L$(OO_PS4_TOOLCHAIN)/lib \
           -lSceSysmodule -lSceUserService -lScePad -lc -lkernel

all: $(OUT_PKG)

$(ELF): $(SRC)
	@mkdir -p build pkg_dir/sce_sys
	$(CC) $(CFLAGS) -c $< -o build/main.o
	$(LD) build/main.o $(LDFLAGS) -o $(ELF)

$(BIN): $(ELF)
	$(OO_PS4_TOOLCHAIN)/bin/linux/x86_64/elf-to-oelf $(ELF) $(OELF)
	$(FSELF) -in $(OELF) -out $(BIN)

$(GP4_XML): $(BIN)
	$(GP4) --out $(GP4_XML) pkg_dir

$(OUT_PKG): $(GP4_XML)
	$(PKGTOOL) --out $(OUT_PKG) $(GP4_XML)

clean:
	rm -rf build pkg_dir/*.bin $(GP4_XML) $(OUT_PKG)

