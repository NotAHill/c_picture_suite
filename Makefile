CC = gcc
CFLAGS = -g -Wall -Werror -Wextra -pedantic
TARGET = main
SRC = main.c bmp.c colour.c picture.c processor.c
OBJDIR = obj
OBJ = $(addprefix $(OBJDIR)/, $(SRC:.c=.o))

all: $(TARGET)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

$(OBJDIR)/%.o: %.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(TARGET) $(OBJDIR)
