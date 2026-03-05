CFLAGS      ?= -O2

RISCV_CFLAGS  := -march=rv32g_zbb_zbs -D'CRYPTO_NAMESPACE(x)=x' -fno-builtin
RISCV_LDFLAGS := -nostartfiles -nostdlib

RISCV_ELF := firmware.elf
RISCV_BIN := firmware.bin

TOOLCHAIN_PATH := /opt/riscv/bin/

RISCV_GCC     = $(TOOLCHAIN_PATH)riscv32-unknown-elf-gcc
RISCV_OBJCOPY = $(TOOLCHAIN_PATH)riscv32-unknown-elf-objcopy
OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

RISCV_OBJECTS = $(filter-out main_riscv.o,$(patsubst %.c, %_riscv.o, $(wildcard *.c)))

.PHONY: RISCV
.PRECIOUS:

all: clean $(RISCV_BIN)
dump: riscv-dump

#RISCV COMPILATION

%_riscv.o: %.c $(HEADERS)
	$(RISCV_GCC) $(CFLAGS) $(RISCV_CFLAGS) -c $< -o $@ -T default.lds

$(RISCV_ELF): main_riscv.o $(RISCV_OBJECTS)
	$(RISCV_GCC) $(RISCV_LDFLAGS) main_riscv.o $(RISCV_OBJECTS) -o $(RISCV_ELF) -T default.lds

$(RISCV_BIN): $(RISCV_ELF)
	${RISCV_OBJCOPY} -O binary $(RISCV_ELF) $@

riscv-dump:
	$(RISCV_OBJDUMP) -D $(RISCV_ELF) > $(RISCV_ELF).lst
	$(RISCV_OBJDUMP) -b binary -m riscv -D $(RISCV_BIN) > $(RISCV_BIN).lst



.PHONY: install

clean:
	@-rm -f *.o
	@-rm -f *.lst
	@-rm -f *.elf
