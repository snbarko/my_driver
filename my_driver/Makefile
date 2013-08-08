
MODULES += driver
MODULES += application

all: module copy

module:

	@echo "Compiling modules: "
	@for i in $(MODULES); do cd $$i; $(MAKE) all;cd ..;done

copy:
	@echo "Copying binaries"
	@cp driver/*.ko bin/
	@cp application/dcli bin/

clean:
	@echo "Cleaning modules: "
	@for i in $(MODULES); do cd $$i; $(MAKE) clean;cd ..;done
	@rm -f bin/*

