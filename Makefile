RM      := rm
CC      := clang
CFLAGS  := -std=gnu99 -Wall -O2
LDFLAGS :=
SOURCES := $(shell find . -name "*.c" -type f -print)
OBJECTS := $(SOURCES:%.c=%.o)
DEPS    := $(OBJECTS:%.o=%.d)
PROG    := cmp

all: $(PROG)

$(PROG): $(OBJECTS)
	@echo "LD		$@"
	@$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	@echo "CC		$@"
	@$(CC) $(CFLAGS) -c -o $@ $<

%.d: %.c
	@echo "DEP		$@"
	@$(CC) $(CFLAGS) -MM -MF $@ -MT $*.o $<

clean:
	@echo "CLEAN"
	@$(RM) -f *.o *.d main

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif

.PHONY: all clean
