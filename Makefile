# nw2s::libsoundplane - soundplane driver for ARMHF
# Copyright (C) 2013 Scott Wilson (thomas.scott.wilson@gmail.com)
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

# Compiler and Linker
CC          := g++

# Target name
TARGET      := libsoundplane.so

#The Directories, Source, Includes, Objects, Binary and Resources
SRCDIR := src
INCDIR := inc
BUILDDIR := obj
TARGETDIR := bin
SRCEXT := cpp
DEPEXT := d
OBJEXT := o

#Flags, Libraries and Includes
CFLAGS      := -Wno-unknown-pragmas -ftree-vectorize -mfloat-abi=hard -O3 -g -std=c++11 -fPIC -march=native -mfpu=neon
LIB         := -lusb-1.0 -lpthread -latomic -lpython2.7 -lboost_python -lboost_system
INC         := -I$(INCDIR) -I/usr/include/python2.7 -I/usr/local/include -I/usr/include/libusb-1.0 
INCDEP      := -I$(INCDIR)

#---------------------------------------------------------------------------------
#---------------------------------------------------------------------------------

SOURCES     := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS     := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.$(OBJEXT)))

# Makes all C
all: $(TARGET)

# Remake
remake: cleaner all

# Clean only Objecst
clean:
	@$(RM) -rf $(BUILDDIR)

# Full Clean, Objects and Binaries
cleaner: clean
	@$(RM) -rf $(TARGETDIR)

# Pull in dependency info for *existing* .o files
-include $(OBJECTS:.$(OBJEXT)=.$(DEPEXT))

# Cython
cython:
	cython --cplus -o src/python/libsoundplane.cpp src/python/libsoundplane.pyx

# Link
$(TARGET): $(OBJECTS)
	@mkdir -p $(TARGETDIR)
	@mkdir -p $(BUILDDIR)
	$(CC) -o $(TARGETDIR)/$(TARGET) -shared $^ $(LIB)

# Compile
$(BUILDDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<
	@$(CC) $(CFLAGS) $(INCDEP) -MM $(SRCDIR)/$*.$(SRCEXT) > $(BUILDDIR)/$*.$(DEPEXT)
	@cp -f $(BUILDDIR)/$*.$(DEPEXT) $(BUILDDIR)/$*.$(DEPEXT).tmp
	@sed -e 's|.*:|$(BUILDDIR)/$*.$(OBJEXT):|' < $(BUILDDIR)/$*.$(DEPEXT).tmp > $(BUILDDIR)/$*.$(DEPEXT)
	@sed -e 's/.*://' -e 's/\\$$//' < $(BUILDDIR)/$*.$(DEPEXT).tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(BUILDDIR)/$*.$(DEPEXT)
	@rm -f $(BUILDDIR)/$*.$(DEPEXT).tmp

# Install
install: 
	cp bin/libsoundplane.so /usr/local/lib/

# Build test executable
test: $(TARGET)
	$(CC) -O3 -g -std=c++11 test/soundplanetest.cpp -lsoundplane -o bin/test-soundplane
	$(CC) -ftree-vectorize -mfloat-abi=hard -O3 -g -std=c++11 -march=native -mfpu=neon test/touchtrackertest.cpp -lsoundplane -o bin/test-tracker


# Non-File Targets
.PHONY: all remake clean cleaner resources