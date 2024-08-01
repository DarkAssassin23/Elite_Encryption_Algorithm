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

#Class Files
CLS = $(patsubst %.java, $(BINDIR)/%.class, $(SRCS))

default: $(CLS)
all: jar

$(CLS): $(BINDIR)/%.class: %.java
	@mkdir -p $(@D)
	$(JC) $(JCFLAGS) $<
	
jar: $(CLS)
	jar cfe $(JAR) $(MAIN) -C $(BINDIR) .
	
clean:
	$(RM) -r $(BINDIR) $(JAR)
