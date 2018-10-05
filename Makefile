
MAKE = /usr/bin/make

all:
	$(MAKE) -C comp/

clean:
	$(MAKE) clean -C comp/
	$(MAKE) clean -C memory/
	
