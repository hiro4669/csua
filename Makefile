
MAKE = /usr/bin/make

all:
	$(MAKE) -C svm/
	$(MAKE) -C comp/
	$(MAKE) -C memory/
	

clean:
	$(MAKE) clean -C comp/
	$(MAKE) clean -C memory/
	$(MAKE) clean -C svm/
	
