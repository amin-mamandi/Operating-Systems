FIRST_NAME=Amin
LAST_NAME=Mamandipoor
KUID=3073224
LAB=08
TAR_BASENAME=Lab$(LAB)_$(FIRST_NAME)_$(LAST_NAME)_$(KUID)

DELIVERABLES=./libpriqueue queuetest.c

all: queuetest

test: queuetest
	./queuetest

queuetest: queuetest.c ./libpriqueue/libpriqueue.c ./libpriqueue/libpriqueue.h
	gcc -g -o $@ $^

libpriqueue.o: ./libpriqueue/libpriqueue.c ./libpriqueue/libpriqueue.h
	gcc -g -c -o $@ $<

tar: clean
#	create temp dir
	mkdir $(TAR_BASENAME)
#	copy the necessary files into the temp dir
	cp -r $(DELIVERABLES) Makefile $(TAR_BASENAME)
#	create the submission tar.gz
	tar cvzf $(TAR_BASENAME).tar.gz $(TAR_BASENAME)
#	remove the temp dir
	rm -rf $(TAR_BASENAME)

clean:
	rm -f *.o queuetest $(TAR_BASENAME)*

.PHONY: clean tar
