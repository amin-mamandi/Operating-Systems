FIRST_NAME=Amin
LAST_NAME=Mamandipoor
KUID=3073224
LAB=02
TAR_BASENAME=Lab$(LAB)_$(FIRST_NAME)_$(LAST_NAME)_$(KUID)

all: signals

signals: signals.c
	gcc -g $^ -o $@

clean:
	rm -rf *.o signals $(TAR_BASENAME) $(TAR_BASENAME).tar.gz

tar: clean
#	create temp dir
	mkdir $(TAR_BASENAME)
#	copy the necessary files into the temp dir
	cp signals.c Makefile $(TAR_BASENAME)
#	create the submission tar.gz
	tar cvzf $(TAR_BASENAME).tar.gz $(TAR_BASENAME)
#	remove the temp dir
	rm -rf $(TAR_BASENAME)

.PHONY: clean tar
