CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
SRC = src/main.c src/args.c src/loader.c src/generator.c src/output.c
OBJ = $(SRC:.c=.o)
TARGET = namegen.exe

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	del src\*.o $(TARGET)
