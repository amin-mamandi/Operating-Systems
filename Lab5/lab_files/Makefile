FIRST_NAME=Amin
LAST_NAME=Mamandipoor
KUID=3073224
LAB=05
TAR_BASENAME=Lab$(LAB)_$(FIRST_NAME)_$(LAST_NAME)_$(KUID)

DELIVERABLES=ptcount_mutex.c ptcount_atomic.c

CCFLAGS = -pedantic -Wall -std=gnu11

LOOP=100000000
LOOP_HELGRIND=1
INC=1



all: ptcount_mutex ptcount_atomic

ptcount_mutex: ptcount_mutex.c
	gcc $(CCFLAGS) -g -o $@ $^ -lpthread

ptcount_atomic: ptcount_atomic.c
	gcc $(CCFLAGS) -g -o $@ $^ -lpthread

test: all
	./ptcount_mutex $(LOOP) $(INC)
	./ptcount_atomic $(LOOP) $(INC)

# test-helgrind: all
# 	valgrind --tool=helgrind ./ptcount_mutex $(LOOP_HELGRIND) $(INC)
# 	valgrind --tool=helgrind ./ptcount_atomic $(LOOP_HELGRIND) $(INC)

clean:
	rm -f ptcount_mutex ptcount_atomic

tar: clean
#	create temp dir
	mkdir $(TAR_BASENAME)
#	copy the necessary files into the temp dir
	cp $(DELIVERABLES) Makefile $(TAR_BASENAME)
#	create the submission tar.gz
	tar cvzf $(TAR_BASENAME).tar.gz $(TAR_BASENAME)
#	remove the temp dir
	rm -rf $(TAR_BASENAME)

.PHONY: clean tar

