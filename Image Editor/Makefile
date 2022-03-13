# compiler setup
CC=gcc
CFLAGS=-Wall -Wextra -std=c99

# define targets
TARGET=image_editor

build: $(TARGET)

image_editor:
	$(CC) $(CFLAGS) *.c -o image_editor -lm

pack:
	zip -FSr 313CA_ManeaDorinMihai_Tema3.zip README Makefile *.c *.h

clean:
	rm -f $(TARGET)

.PHONY: pack clean
