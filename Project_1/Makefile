FIRST_NAME=Amin
LAST_NAME=Mamandipoor
KUID=3073224
Project=1
TAR_BASENAME=Project$(Project)_$(FIRST_NAME)_$(LAST_NAME)_$(KUID)

DELIVERABLES=quash.c

all: quash

finder: quash.c
        gcc -g $^ -o $@

clean:
        rm -rf *.o quash $(TAR_BASENAME) $(TAR_BASENAME).tar.gz

tar: clean
#       create temp dir
        mkdir $(TAR_BASENAME)
#       copy the necessary files into the temp dir
        cp $(DELIVERABLES) Makefile $(TAR_BASENAME)
#       create the submission tar.gz
        tar cvzf $(TAR_BASENAME).tar.gz $(TAR_BASENAME)
#       remove the temp dir
        rm -rf $(TAR_BASENAME)

.PHONY: clean tar test

