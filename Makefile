CC=gcc
CCFLAGS=-g -Wall -pedantic -std=c11
LDFLAGS=-lzmq
TESTLDFLAGS=-lcheck -lm -lpthread -lrt

TARGET=chuckdaw

SRC=$(wildcard src/**/*.c src/*.c)
OBJ=$(SRC:%.c=%.o)

OBJWITHOUTMAIN := $(filter-out src/main.o,$(OBJ))

TESTSRC=$(wildcard tests/**/*.c tests/*.c)
TESTOBJ=$(TESTSRC:%.c=%.o)

build: test $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CCFLAGS) -o $(TARGET) $^ $(LDFLAGS)

test: $(TESTOBJ) $(OBJWITHOUTMAIN)
	$(CC) $(CCFLAGS) -o test $^ $(LDFLAGS) $(TESTLDFLAGS)

clean:
	rm -f $(TARGET) test $(OBJ) $(TESTOBJ)
