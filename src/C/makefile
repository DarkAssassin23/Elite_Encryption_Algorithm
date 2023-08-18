TARGET = eea

CC = gcc
CFLAGS = -Wall -g
LIBS = -lcrypto #-L logger/libs/debug/static -lfunclogd
INCLUDES = -I headers/ #-I logger/

OBJDIR = obj

all: $(TARGET)

SRCS = $(wildcard src/*.c)
HEADERS = $(wildcard headers/*.h)
OBJS = $(patsubst %.c, $(OBJDIR)/%.o, $(SRCS))

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LIBS) -o $@

$(OBJDIR)/%.o: %.c $(HEADERS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM) -r $(OBJDIR) $(TARGET)