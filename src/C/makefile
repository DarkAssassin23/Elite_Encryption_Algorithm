TARGET = eea

CC = gcc
CFLAGS = -Wall -g -pedantic
LIBS = -lcrypto -lpthread
INCLUDES = -I headers/

OBJDIR = obj

all: $(TARGET)
debug: CFLAGS = -Wall -g -pedantic
debug: $(TARGET)

release: CFLAGS = -O2
release: $(TARGET)

SRCS = $(wildcard src/*.c)
HEADERS = $(wildcard headers/*.h)
OBJS = $(patsubst %.c, $(OBJDIR)/%.o, $(SRCS))
ifeq ($(OS),Windows_NT)
	RCS = $(wildcard version/*.rc)
	RES = $(patsubst %.rc, $(OBJDIR)/%.res, $(RCS))
	DEFINES = -DWIN32
endif

$(TARGET): $(OBJS) $(RES)
	@$(CC) $(OBJS) $(RES) $(LIBS) -o $@
	@echo "Created: "$@

$(OBJDIR)/%.res: %.rc
ifeq ($(OS),Windows_NT)
	@mkdir -p $(@D)
	@windres $(RCSDEBUG) $< -O coff -o $@
	@echo "windres  "$@
endif

$(OBJDIR)/%.o: %.c $(HEADERS)
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) $(INCLUDES) $(DEFINES) -c $< -o $@
	@echo $(CC) "     "$@

clean:
	$(RM) -r $(OBJDIR) $(TARGET)