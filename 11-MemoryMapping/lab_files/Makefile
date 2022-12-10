FIRST_NAME=Amin
LAST_NAME=Mamandipoor
KUID=3073224
LAB=11
TAR_BASENAME=Lab$(LAB)_$(FIRST_NAME)_$(LAST_NAME)_$(KUID)

DELIVERABLES=read_write.c memmap.c sample.ogg

all: read_write memmap

read_write: read_write.c
	gcc -g -o $@ $<

memmap: memmap.c
	gcc -g -o $@ $<

clean:
	rm -f *.o read_write memmap copy.ogg $(TAR_BASENAME)*

test: memmap
	./memmap sample.ogg copy.ogg
	diff sample.ogg copy.ogg

time_compare: all
	@printf "Time of read_write copy (buffer size = 10)...\n"
	time ./read_write sample.ogg copy.ogg 10
	@printf "\nTime of memmap copy...\n"
	time ./memmap sample.ogg copy.ogg

tar: clean
#	create temp dir
	mkdir $(TAR_BASENAME)
#	copy the necessary files into the temp dir
	cp -r $(DELIVERABLES) Makefile $(TAR_BASENAME)
#	create the submission tar.gz
	tar cvzf $(TAR_BASENAME).tar.gz $(TAR_BASENAME)
#	remove the temp dir
	rm -rf $(TAR_BASENAME)

.PHONY: clean test time_compare tar
