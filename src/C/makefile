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
ifeq ($(OS),Windows_NT)
	RCS = $(wildcard version/*.rc)
	RES = $(patsubst %.rc, $(OBJDIR)/%.res, $(RCS))
endif

$(TARGET): $(OBJS) $(RES)
	$(CC) $(OBJS) $(RES) $(LIBS) -o $@

$(OBJDIR)/%.res: %.rc
ifeq ($(OS),Windows_NT)
	@mkdir -p $(@D)
	windres $(RCSDEBUG) $< -O coff -o $@
endif

$(OBJDIR)/%.o: %.c $(HEADERS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM) -r $(OBJDIR) $(TARGET)