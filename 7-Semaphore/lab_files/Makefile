FIRST_NAME=Amin
LAST_NAME=Mamandipoor
KUID=3073224
LAB=07
TAR_BASENAME=Lab$(LAB)_$(FIRST_NAME)_$(LAST_NAME)_$(KUID)

DELIVERABLES=producer_consumer.c
CMD=./producer_consumer

all: producer_consumer

producer_consumer: producer_consumer.c
	gcc -g -o $@ $< -lpthread -lm

tar: clean
#	create temp dir
	mkdir $(TAR_BASENAME)
#	copy the necessary files into the temp dir
	cp $(DELIVERABLES) Makefile $(TAR_BASENAME)
#	create the submission tar.gz
	tar cvzf $(TAR_BASENAME).tar.gz $(TAR_BASENAME)
#	remove the temp dir
	rm -rf $(TAR_BASENAME)

all-tests: test1 test2 test3 test4

test1: test1-simple
test2: test2-simple
test3: test3-simple
test4: test4-simple

helgrind-tests: CMD = valgrind --tool=helgrind ./producer_consumer
helgrind-tests: test1-simple test2-simple test3-simple test4-simple

test1-simple: producer_consumer
	@echo "  P-1 C-1   " > narrative1.raw
	@echo "************" >> narrative1.raw
	@echo "    RAW     " >> narrative1.raw
	@echo "************" >> narrative1.raw
	@$(CMD) 1 1 >> narrative1.raw
	@echo "  P-1 C-1   " > narrative1.sorted
	@echo "************" >> narrative1.sorted
	@echo "  Sorted    " >> narrative1.sorted
	@echo "************" >> narrative1.sorted
	@grep "con 0"  narrative1.raw >> narrative1.sorted
	@echo "    " >> narrative1.sorted
	@grep "prod 0" narrative1.raw >> narrative1.sorted

test2-simple: producer_consumer
	@echo "  P-1 C-5   " > narrative2.raw
	@echo "************" >> narrative2.raw
	@echo "    RAW     " >> narrative2.raw
	@echo "************" >> narrative2.raw
	@$(CMD) 1 5 >> narrative2.raw
	@echo "  P-1 C-5   " > narrative2.sorted
	@echo "************" >> narrative2.sorted
	@echo "  Sorted    " >> narrative2.sorted
	@echo "************" >> narrative2.sorted
	@grep "con 0"  narrative2.raw >> narrative2.sorted
	@echo "    " >> narrative2.sorted
	@grep "con 1"  narrative2.raw >> narrative2.sorted
	@echo "    " >> narrative2.sorted
	@grep "con 2"  narrative2.raw >> narrative2.sorted
	@echo "    " >> narrative2.sorted
	@grep "con 3"  narrative2.raw >> narrative2.sorted
	@echo "    " >> narrative2.sorted
	@grep "con 4"  narrative2.raw >> narrative2.sorted
	@echo "    " >> narrative2.sorted
	@grep "prod 0" narrative2.raw >> narrative2.sorted

test3-simple: producer_consumer
	@echo "  P-5 C-1   " > narrative3.raw
	@echo "************" >> narrative3.raw
	@echo "    RAW     " >> narrative3.raw
	@echo "************" >> narrative3.raw
	@$(CMD) 5 1 >> narrative3.raw
	@echo "  P-5 C-1   " > narrative3.sorted
	@echo "************" >> narrative3.sorted
	@echo "  Sorted    " >> narrative3.sorted
	@echo "************" >> narrative3.sorted
	@grep "con 0"  narrative3.raw >> narrative3.sorted
	@echo "    " >> narrative3.sorted
	@grep "prod 0" narrative3.raw >> narrative3.sorted
	@echo "    " >> narrative3.sorted
	@grep "prod 1" narrative3.raw >> narrative3.sorted
	@echo "    " >> narrative3.sorted
	@grep "prod 2" narrative3.raw >> narrative3.sorted
	@echo "    " >> narrative3.sorted
	@grep "prod 3" narrative3.raw >> narrative3.sorted
	@echo "    " >> narrative3.sorted
	@grep "prod 4" narrative3.raw >> narrative3.sorted

test4-simple: producer_consumer
	@echo "  P-5 C-5   " > narrative4.raw
	@echo "************" >> narrative4.raw
	@echo "    RAW     " >> narrative4.raw
	@echo "************" >> narrative4.raw
	@$(CMD) 5 5 >> narrative4.raw
	@echo "  P-5 C-5   " > narrative4.sorted
	@echo "************" >> narrative4.sorted
	@echo "  Sorted    " >> narrative4.sorted
	@echo "************" >> narrative4.sorted
	@grep "con 0"  narrative4.raw >> narrative4.sorted
	@echo "    " >> narrative4.sorted
	@grep "con 1"  narrative4.raw >> narrative4.sorted
	@echo "    " >> narrative4.sorted
	@grep "con 2"  narrative4.raw >> narrative4.sorted
	@echo "    " >> narrative4.sorted
	@grep "con 3"  narrative4.raw >> narrative4.sorted
	@echo "    " >> narrative4.sorted
	@grep "con 4"  narrative4.raw >> narrative4.sorted
	@echo "    " >> narrative4.sorted
	@grep "prod 0" narrative4.raw >> narrative4.sorted
	@echo "    " >> narrative4.sorted
	@grep "prod 1" narrative4.raw >> narrative4.sorted
	@echo "    " >> narrative4.sorted
	@grep "prod 2" narrative4.raw >> narrative4.sorted
	@echo "    " >> narrative4.sorted
	@grep "prod 3" narrative4.raw >> narrative4.sorted
	@echo "    " >> narrative4.sorted
	@grep "prod 4" narrative4.raw >> narrative4.sorted

clean:
	rm -f *~ *.raw *.sorted producer_consumer $(TAR_BASENAME)*

.PHONY: clean tar
