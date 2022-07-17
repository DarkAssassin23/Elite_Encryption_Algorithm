JAR = 'EEA_App.jar'
JC = javac

# Source Dirs
SRC = src
APPDIR = $(SRC)/app
UTILDIR = $(SRC)/encryptionUtilities

MAIN = app/AppUI

#Class Files Directory
BINDIR = bin

JCFLAGS = -cp $(SRC) -d $(BINDIR)

#Source Files
SRCS = $(wildcard $(UTILDIR)/*.java $(APPDIR)/*.java)

#$(info $(SRCS))

#Class Files
CLS = $(patsubst %.java, $(BINDIR)/%.class, $(SRCS))

default: $(CLS)
all: jar

$(CLS): $(BINDIR)/%.class: %.java
ifeq ($(OS),Windows_NT)
	@if not exist $(@D) @mkdir $(@D)
else
	@mkdir -p $(@D)
endif
	$(JC) $(JCFLAGS) $<
	
jar: $(CLS)
	jar cfe $(JAR) $(MAIN) -C $(BINDIR) .
	
clean:
ifeq ($(OS),Windows_NT)
	-rmdir $(BINDIR) /q /s 2>nul && del $(JAR) /q /f 2>nul
else
	-rm -rf $(BINDIR) $(JAR)
endif
