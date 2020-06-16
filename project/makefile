# change application name here (executable output name)
TARGET=project

# compiler
CC=gcc
# debug
DEBUG=-g
# optimisation
OPT=-O0
# warnings
WARN=-Wall

PTHREAD=-pthread

CCFLAGS=$(DEBUG) $(OPT) $(WARN) $(PTHREAD) -pipe

GTKLIB=`pkg-config --cflags --libs gtk+-3.0`

LIBBTREE=lib/bt-5.0.0/lib/libbt.a

LIBBJRB=lib/libfdr/libfdr.a

# linker
LD=gcc
LDFLAGS=$(PTHREAD) $(GTKLIB) -export-dynamic

OBJS=    main.o

all: $(OBJS)
	$(LD) -o $(TARGET) $(OBJS) $(LDFLAGS) $(LIBBTREE) $(LIBBJRB)
    
main.o: main.c
	$(CC) -c $(CCFLAGS) main.c $(GTKLIB)  -o main.o 
    
clean:
	rm -f *.o *.out words.dat $(TARGET)

