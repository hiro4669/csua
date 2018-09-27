TARGET = scant
CC = /usr/bin/gcc
CFLAGS = -g -DDEBUG
OBJS = y.tab.o scanner.o keyword.o
EXEC = scantest.o

YACC = csua.y

all: $(OBJS) $(EXEC)
	$(CC) -o $(TARGET) $^


keyword.c: keyword.key
	gperf -tT $^ > $@
	
y.tab.c: $(YACC)
	bison --yacc -dv $^

.c.o:
	$(CC) -c $(CFLAGS) $*.c

clean:
	rm -rf $(TARGET) *~ keyword.c *.o y.tab.h y.tab.c y.output
