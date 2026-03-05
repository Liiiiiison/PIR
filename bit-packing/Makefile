CFLAGS      ?= -O2 -L../../lib/ -I../../include/
DEBUG_CFLAGS = -Wall -g

x64_TARGET  := imprise
x64_GNU     :=
x64_GCC     := $(PREFIX)gcc -m64
x64_OBJDUMP := $(PREFIX)objdump
x64_OBJCOPY := $(PREFIX)objcopy
x64_GDB     := $(PREFIX)gdb

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

x64_OBJECTS = $(patsubst %.c, %_x64.o, $(wildcard *.c))

.PHONY: x64
.PRECIOUS:

all: clean x64
dump: x64-dump
	
#x64 COMPILATION

%_x64.o: %.c $(HEADERS)
	$(x64_GCC) $(CFLAGS) $(DEBUG_CFLAGS) $(x64_CFLAGS) -c $< -o $@

x64: $(x64_OBJECTS)
	$(x64_GCC) $(CFLAGS) $(x64_OBJECTS) -o $(x64_TARGET) -lm -limprise

x64-dump:
	$(x64_OBJDUMP) -d $(x64_TARGET) > $(x64_TARGET).lst

.PHONY: install

clean:
	@-rm -f *.o
	@-rm -f *.lst
	@-rm -f *.bin
