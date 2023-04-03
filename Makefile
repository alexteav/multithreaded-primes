CC = gcc
DEBUG = -g
CFLAGS = $(DEBUG) \
		 -Wall \
		 -Wshadow \
		 -Wextra \
		 -Wunreachable-code \
		 -Wredundant-decls \
		 -Wmissing-declarations \
		 -Wold-style-definition \
		 -Wmissing-prototypes \
		 -Wdeclaration-after-statement \
		 -Wno-return-local-addr \
		 -Wunsafe-loop-optimizations \
		 -Wuninitialized \

#CFLAGS += -Werror # for treating warnings as compilation errors


PROG = primesMT
AOUT = primesMT
TAR_FILE = ${LOGNAME}_$(PROG).tar.gz

all: $(PROG)

$(PROG): $(PROG).o
	$(CC) $(CFLAGS) -o $(AOUT) $(PROG).o -lm -pthread
	chmod og-rx $(AOUT)

# -lm is used for sqrt() function
$(PROG).o: $(PROG).c
	$(CC) $(CFLAGS) -c -o $(PROG).o $(PROG).c -lm -pthread


opt: clean
	make DEBUG=-O3

tar: clean
	tar cvfa $(PROG).tar.gz *.[ch] ?akefile

clean:
	rm -f $(PROG) *.o *~ \#*

