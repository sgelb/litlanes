CC := clang++
SRCDIR := src
BUILDDIR := build
# http://photography.nationalgeographic.com/photography/photo-of-the-day/litlanesfoss-waterfall-iceland/
TARGET := litlanes
DESTDIR ?= /local/usr

SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CXXFLAGS := -pedantic -g -Wall -Wextra
DEBUGFLAGS := -O0 -g
RELEASEFLAGS := -O2 -g0
LDFLAGS	= -std=c++11
LIB := -L lib $(shell pkg-config --libs glew gl glfw3) -lnoise
INC := -I include
DEPENDFILE := Makefile.dep

# default target
default	: release
all	: $(TARGET)

release: CXXFLAGS += $(RELEASEFLAGS)
release: $(TARGET)

debug: CXXFLAGS += $(DEBUGFLAGS)
debug: $(TARGET)

# dependencies collected with cc -MM
-include $(DEPENDFILE)
$(DEPENDFILE)	: $(SOURCES)
	$(CC) -MM $(INC) $(LDFLAGS) $(SOURCES) > $(DEPENDFILE)

$(TARGET): $(OBJECTS)
	@echo " Linking..."
	$(CC) $^ -o $(TARGET) $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	$(CC) $(LDFLAGS) $(CXXFLAGS) $(INC) -c -o $@ $<

install: release
	install -m 0755 -D $(TARGET) $(DESTDIR)/$(TARGET)

clean:
	@echo " Cleaning..."; 
	$(RM) -r $(BUILDDIR) $(TARGET) $(DEPENDFILE)

.PHONY: release debug install clean
