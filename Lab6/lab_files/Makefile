FIRST_NAME=Jane
LAST_NAME=Doe
KUID=XXXXXXX
LAB=06
TAR_BASENAME=Lab$(LAB)_$(FIRST_NAME)_$(LAST_NAME)_$(KUID)

SRCS=dp_asymmetric.c dp_waiter.c
DELIVERABLES=$(SRCS)
BINS=$(SRCS:.c=)

all: dp_asymmetric dp_waiter

# dp_asymmetric, dp_waiter
%: %.c
	gcc -g $< -lpthread -lm -o $@

test: dp_asymmetric_test dp_waiter_test

dp_asymmetric_test: dp_asymmetric
	@echo "$@..."
	./$<

dp_waiter_test: dp_waiter
	@echo "$@..."
	./$<

clean:
	rm -rf *.o $(BINS) $(TAR_BASENAME)*

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
