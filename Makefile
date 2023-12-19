CC = gcc
CFLAGS = -g -Wall -Werror -Wextra -pedantic
TARGET = main
SRC = main.c bmp.c colour.c picture.c
OBJ = $(SRC:.c=.o)

all: $(TARGET)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(TARGET) *.o core
