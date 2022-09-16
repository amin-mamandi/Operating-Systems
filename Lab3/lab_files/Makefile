FIRST_NAME=Amin
LAST_NAME=Mamandipoor
KUID=3073224
LAB=03
TAR_BASENAME=Lab$(LAB)_$(FIRST_NAME)_$(LAST_NAME)_$(KUID)

DELIVERABLES=finder.c

all: finder

finder: finder.c
	gcc -g $^ -o $@

test: finder
	./test.sh ./bash-4.2 array 5

clean:
	rm -rf *.o finder $(TAR_BASENAME) $(TAR_BASENAME).tar.gz desired.txt output.txt

tar: clean
#	create temp dir
	mkdir $(TAR_BASENAME)
#	copy the necessary files into the temp dir
	cp $(DELIVERABLES) Makefile $(TAR_BASENAME)
#	create the submission tar.gz
	tar cvzf $(TAR_BASENAME).tar.gz $(TAR_BASENAME)
#	remove the temp dir
	rm -rf $(TAR_BASENAME)

.PHONY: clean tar test
