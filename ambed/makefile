CC=g++
CFLAGS=-c -std=c++11 -pthread
LDFLAGS=-std=c++11 -pthread 
SOURCES=$(wildcard *.cpp)
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=ambed

PREFIX?=/usr/local
EXEC_PREFIX?=$(PREFIX)
SBIN_DIR?=$(EXEC_PREFIX)sbin
SYSCONF_DIR?=etc
INSTALL?=install
INSTALL_PROGRAM?=$(INSTALL)
INSTALL_DATA?=${INSTALL} -m 644


all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -lftd2xx -Wl,-rpath,/usr/local/lib -lsystemd -o $@


.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	$(RM) $(EXECUTABLE) *.o

install:
	$(INSTALL) -d $(DESTDIR)$(SBIN_DIR)
	$(INSTALL_PROGRAM) $(EXECUTABLE) $(DESTDIR)$(SBIN_DIR)/$(EXECUTABLE)
	$(INSTALL_PROGRAM) run $(DESTDIR)$(SBIN_DIR)/
