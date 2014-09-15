
CFLAGS=-Wall
OBJECS=memlib.o csapp.o mm.o

CC=gcc
AR=ar
TARGETS=mm.a

all:mm.a
mm.a:$(OBJECS)
	$(AR) -rc $(TARGETS) $(OBJECS)
$(OBJECS):%.o:%.c
	$(CC) -c $(CFLAGS) $< -o $@ -lpthread
.PHONY:clean
clean:
	rm -rf *.o *.a
