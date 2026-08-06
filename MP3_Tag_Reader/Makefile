CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = mp3_tag_reader
SRCS = main.c view.c edit.c
OBJS = $(SRCS:.c=.o)
HEADERS = types.h view.h edit.h

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

$(OBJS): %.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all run clean
