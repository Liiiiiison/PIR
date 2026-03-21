CFLAGS      ?= -O2 -L../../lib/ -I../../include/
DEBUG_CFLAGS = -Wall -g

x64_TARGET  := imprise
x64_GNU     :=
x64_GCC     := $(PREFIX)gcc -m64 -Wno-implicit-function-declaration -Wno-unused-variable
x64_OBJDUMP := $(PREFIX)objdump
x64_OBJCOPY := $(PREFIX)objcopy
x64_GDB     := $(PREFIX)gdb

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

x64_128_OBJECTS = $(patsubst %.c, %_128.o, $(wildcard *.c))
x64_256_OBJECTS = $(patsubst %.c, %_256.o, $(wildcard *.c))

.PHONY: imprise-128 no-simd
.PRECIOUS:

all: clean no-simd

no-simd: clean imprise-128
	make -C firmware/ TOOLCHAIN_PATH=/opt/riscv/bin/ RISCV_CFLAGS="-march=rv32im_zbb_zbs_zicsr -mabi=ilp32 -fno-builtin" RISCV_LDFLAGS="-march=rv32im_zbb_zbs_zicsr -mabi=ilp32 -nostartfiles -nostdlib"
	
imprise-128: $(x64_128_OBJECTS)
	$(x64_GCC) $(CFLAGS) $(x64_128_OBJECTS) -o $(x64_TARGET) -lm -limprise
	
imprise-256: $(x64_256_OBJECTS)
	$(x64_GCC) $(CFLAGS) $(x64_256_OBJECTS) -o $(x64_TARGET) -lm -limprise
	
%_128.o: %.c $(HEADERS)
	$(x64_GCC) $(CFLAGS) -DRISCV_VLEN=128 $(DEBUG_CFLAGS) $(x64_CFLAGS) -c $< -o $@
	
%_256.o: %.c $(HEADERS)
	$(x64_GCC) $(CFLAGS) -DRISCV_VLEN=256 $(DEBUG_CFLAGS) $(x64_CFLAGS) -c $< -o $@

clean:
	@-rm -f *.o
	@-rm -f *.lst
	@-rm -f $(x64_TARGET)
	@-rm -f firmware/*.o
	@-rm -f firmware/*.lst
	@-rm -f firmware/*.bin
	@-rm -f firmware/*.elf
