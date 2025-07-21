all:
	$(MAKE) -C src
	$(MAKE) -C echo	

clean:
	$(MAKE) -C src clean
	$(MAKE) -C echo clean

install:
	$(MAKE) -C src install
	$(MAKE) -C echo install

.PHONY: all clean install
