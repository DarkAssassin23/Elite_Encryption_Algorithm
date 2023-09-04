TARGET = eea

CC = gcc
CFLAGS = -Wall -g
LIBS = -lcrypto -lpthread
INCLUDES = -I headers/

OBJDIR = obj

all: $(TARGET)
debug: CFLAGS = -Wall -g
debug: $(TARGET)

release: CFLAGS = -O2
release: $(TARGET)

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