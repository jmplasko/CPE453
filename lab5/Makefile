CC 	= gcc

CFLAGS  = -Wall -g

LD 	= gcc

LDFLAGS  = -Wall -g

PUBFILES =  README  hungrymain.c  libPLN.a  libsnakes.a  lwp.h\
	    numbersmain.c  snakemain.c  snakes.h

PROGS	= snakes nums hungry

SNAKEOBJS  = snakemain.o 

HUNGRYOBJS = hungrymain.o 

NUMOBJS    = numbersmain.o

OBJS	= $(SNAKEOBJS) $(HUNGRYOBJS) $(NUMOBJS) 

SRCS	= snakemain.c numbersmain.c hungrymain.c

HDRS	= 

EXTRACLEAN = core $(PROGS)

all: 	$(PROGS)

allclean: clean
	@rm -f $(EXTRACLEAN)

clean:	
	rm -f $(OBJS) *~ TAGS

snakes: snakemain.o libsnakes.a
	$(LD) $(LDFLAGS) -o snakes snakemain.c lwp.c magic64.S libsnakes.a -lncurses

hungry: hungrymain.o libsnakes.a
	$(LD) $(LDFLAGS) -o hungry hungrymain.c lwp.c magic64.S libsnakes.a -lncurses

nums: numbersmain.o
	$(LD) $(LDFLAGS) -o nums numbersmain.c lwp.c magic64.S

