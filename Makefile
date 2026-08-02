CC = clang
CFLAGS = -g -Wall -Wextra -Isrc/include
LDLIBS = -lm

SRC = src/main.c src/num_vec.c
OBJ = $(SRC:.c=.o)
TARGET = main

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJ) $(TARGET)
