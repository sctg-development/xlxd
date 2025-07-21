CC=g++
CFLAGS=-c -std=c++11 -pthread
LDFLAGS=-std=c++11 -pthread 
SOURCES=$(wildcard *.cpp)
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=ambedtest

PREFIX?=/usr/local
EXEC_PREFIX?=$(PREFIX)
BIN_DIR?=$(EXEC_PREFIX)bin
SYSCONF_DIR?=etc
INSTALL?=install
INSTALL_PROGRAM?=$(INSTALL)
INSTALL_DATA?=${INSTALL} -m 644

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -Wl,-rpath,/usr/local/lib -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	$(RM) $(EXECUTABLE) *.o

.PHONY: install
install:
	$(INSTALL) -d $(DESTDIR)$(BIN_DIR)
	$(INSTALL_PROGRAM) $(EXECUTABLE) $(DESTDIR)$(BIN_DIR)/$(EXECUTABLE)
