CC=clang
DEFINES=-DPAK_VERBOSE
CFLAGS=-g -std=c99 -O2 -pipe -Wall -Wextra -Wformat -fno-strict-aliasing ${INCLUDES} ${DEFINES}
INCLUDES=-I. -Itest

SOURCES=$(wildcard *.c test/*.c)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))

TARGET=a.out
$(TARGET): $(OBJECTS)
	@echo '!!! NOTE: This makefile is for the unit tests!                       !!!'
	@echo '!!! If you are a user, there is no need to compile this.             !!!'
	@echo '!!! Just copy and paste the headers into your project and your done! !!!'
	$(CC) -o $(TARGET) $(OBJECTS)

all:
	$(TARGET)

clean:
	rm -rf $(OBJECTS) a.out
	find . -name "*.gc*" -exec rm {} \;
	rm -rf 'find . -name "*.dSYM" -print'
