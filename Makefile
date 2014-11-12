TARGET = 3600dns

default: all

$(TARGET): $(TARGET).c
	gcc -std=c99 -O0 -g -lm -Wall -pedantic -Wextra -o $@ $< helpers.c

all: $(TARGET)

unittests:
	gcc -std=c99 -O0 -g -lm -Wall -pedantic -Wextra helpers.c utests.c -o ./utests

utest: unittests
	./utests

test: all
	./test

debug: unittests
	gdb ./utests

clean:
	rm $(TARGET)

cleanall:
	rm $(TARGET)
	rm utests

