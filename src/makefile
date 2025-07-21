ALL_CXXFLAGS=-std=c++11 -pthread $(CXXFLAGS)
SOURCES=$(wildcard *.cpp)
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=xlxd

# variables for staged installation
# directory variables - https://www.gnu.org/software/make/manual/html_node/Directory-Variables.html
# DESTDIR - https://www.gnu.org/software/make/manual/html_node/DESTDIR.html
# CommandVariables - #https://www.gnu.org/software/make/manual/html_node/Command-Variables.html
PREFIX?=/usr/local
EXEC_PREFIX?=$(PREFIX)
# by default install under /xlxd/ - it should be $(exec_dir)/sbin
#sbin_dir=/xlxd
SBIN_DIR?=$(EXEC_PREFIX)sbin
# by default install config under /xlxd/ - it should be $(PREFIX)/etc
#SYSCONF_DIR=/xlxd
SYSCONF_DIR?=etc/xlxd
SYSCONF_DB_DIR?=etc/xlxd-dashboard
SYSCONF_DB2_DIR?=etc/xlxd-dashboard2
SYSCONF_APACHE2_DIR?=etc/apache2/sites-available
SHARE_DIR?=$(EXEC_PREFIX)share
LOG_DIR?=/var/log/xlxd
VAR_DIR?=/var/lib/xlxd
RUN_DIR?=/var/run/xlxd
INSTALL?=install
INSTALL_PROGRAM?=$(INSTALL)
INSTALL_DATA?=${INSTALL} -m 644

all: $(OBJECTS) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(CPPFLAGS) $(ALL_CXXFLAGS) $(OBJECTS) -lsystemd -o $@

.cpp.o: %.h
	$(CXX) -c $(CPPFLAGS) $(ALL_CXXFLAGS) $<

.PHONY: clean
clean:
	$(RM) $(EXECUTABLE) *.o

.PHONY: install
install:
	$(INSTALL) -d $(DESTDIR)$(SBIN_DIR)
	$(INSTALL) -d $(DESTDIR)$(LOG_DIR)
	$(INSTALL) -d $(DESTDIR)$(VAR_DIR)
	$(INSTALL_PROGRAM) $(EXECUTABLE) $(DESTDIR)$(SBIN_DIR)/$(EXECUTABLE)
	$(INSTALL) -d $(DESTDIR)$(SYSCONF_DIR)
	[ -f $(DESTDIR)$(SYSCONF_DIR)/xlxd.blacklist ] && \
		$(INSTALL_DATA) ../config/xlxd.blacklist  $(DESTDIR)$(SYSCONF_DIR)/xlxd.blacklist.sample || \
		$(INSTALL_DATA) ../config/xlxd.blacklist  $(DESTDIR)$(SYSCONF_DIR)/xlxd.blacklist
	[ -f $(DESTDIR)$(SYSCONF_DIR)/xlxd.whitelist ] && \
		$(INSTALL_DATA) ../config/xlxd.whitelist  $(DESTDIR)$(SYSCONF_DIR)/xlxd.whitelist.sample || \
		$(INSTALL_DATA) ../config/xlxd.whitelist  $(DESTDIR)$(SYSCONF_DIR)/xlxd.whitelist
	[ -f $(DESTDIR)$(SYSCONF_DIR)/xlxd.interlink ] && \
		$(INSTALL_DATA) ../config/xlxd.interlink  $(DESTDIR)$(SYSCONF_DIR)/xlxd.interlink.sample || \
		$(INSTALL_DATA) ../config/xlxd.interlink  $(DESTDIR)$(SYSCONF_DIR)/xlxd.interlink
	[ -f $(DESTDIR)$(SYSCONF_DIR)/xlxd.terminal ] && \
		$(INSTALL_DATA) ../config/xlxd.terminal  $(DESTDIR)$(SYSCONF_DIR)/xlxd.terminal.sample || \
		$(INSTALL_DATA) ../config/xlxd.terminal  $(DESTDIR)$(SYSCONF_DIR)/xlxd.terminal
	[ -f $(DESTDIR)$(SYSCONF_DIR)/xlxd.transcoder ] && \
		$(INSTALL_DATA) ../config/xlxd.transcoder  $(DESTDIR)$(SYSCONF_DIR)/xlxd.transcoder.sample || \
		$(INSTALL_DATA) ../config/xlxd.transcoder  $(DESTDIR)$(SYSCONF_DIR)/xlxd.transcoder
	[ -f $(DESTDIR)$(SYSCONF_DIR)/xlxd.config ] && \
		$(INSTALL_DATA) ../config/xlxd.config  $(DESTDIR)$(SYSCONF_DIR)/xlxd.config.sample || \
		$(INSTALL_DATA) ../config/xlxd.config  $(DESTDIR)$(SYSCONF_DIR)/xlxd.config
	$(INSTALL) -d $(DESTDIR)$(SHARE_DIR)
	$(INSTALL) -d $(DESTDIR)$(SYSCONF_DB_DIR)
	$(INSTALL) -d $(DESTDIR)$(SYSCONF_DB2_DIR)
	$(INSTALL) -d $(DESTDIR)$(SYSCONF_APACHE2_DIR)
	cp -a ../dashboard/. $(DESTDIR)$(SHARE_DIR)/xlxd-dashboard
	mv $(DESTDIR)$(SHARE_DIR)/xlxd-dashboard/pgs/config.inc.php $(DESTDIR)$(SYSCONF_DB_DIR)
	mv $(DESTDIR)$(SHARE_DIR)/xlxd-dashboard/css/layout.css $(DESTDIR)$(SYSCONF_DB_DIR)
	cp -a ../dashboard2/. $(DESTDIR)$(SHARE_DIR)/xlxd-dashboard2
	mv $(DESTDIR)$(SHARE_DIR)/xlxd-dashboard2/pgs/config.inc.php $(DESTDIR)$(SYSCONF_DB2_DIR)
	$(INSTALL_PROGRAM) ../scripts/xlxapi-update $(DESTDIR)$(SBIN_DIR)/
	$(INSTALL_PROGRAM) ../scripts/ambedwatchdog $(DESTDIR)$(SBIN_DIR)/
	$(INSTALL_PROGRAM) ../scripts/xlxd-dashboard.conf $(DESTDIR)$(SYSCONF_APACHE2_DIR)/
	$(INSTALL_PROGRAM) ../scripts/xlxd-dashboard2.conf $(DESTDIR)$(SYSCONF_APACHE2_DIR)/
