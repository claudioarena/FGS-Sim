# ------------------------------------------------
# Generic Makefile
#
# Taken from https://stackoverflow.com/questions/7004702/
# 
# Author: yanick.rochon@gmail.com
# Date  : 2011-08-10
#
# Changelog :
#   2010-11-05 - first version
#   2011-08-10 - added structure : sources, objects, binaries
#                thanks to http://stackoverflow.com/users/128940/beta
#   2017-04-24 - changed order of linker params
#   2017-07-26 - Modified to match FGS-Sim project
# ------------------------------------------------

# project name (generate executable with this name)
TARGET   = CentroidSim

CC       = g++
# compiling flags here
CFLAGS   = -std=c++11 -Wall -I.

LINKER   = g++
# linking flags here
LFLAGS   = -Wall -I. -lm

# change these to proper directories where each file should be
SRCDIR   = src
OBJDIR   = obj
BINDIR   = bin

SOURCES  := $(wildcard $(SRCDIR)/*.c)
INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
rm       = rm -f


$(BINDIR)/$(TARGET): $(OBJECTS)
    @$(LINKER) $(OBJECTS) $(LFLAGS) -o $@
    @echo "Linking complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
    @$(CC) $(CFLAGS) -c $< -o $@
    @echo "Compiled "$<" successfully!"

.PHONY: clean
clean:
    @$(rm) $(OBJECTS)
    @echo "Cleanup complete!"

.PHONY: remove
remove: clean
    @$(rm) $(BINDIR)/$(TARGET)
    @echo "Executable removed!"
