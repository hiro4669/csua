TARGET = scant
CC = /usr/bin/gcc
CFLAGS = -g -DDEBUG
MEMORY = ../memory/memory.o ../memory/storage.o
OBJS = y.tab.o scanner.o keyword.o create.o visitor.o traversor.o
EXEC = scantest.o

YACC = csua.y

treet: $(MEMORY) $(OBJS) treetest.o
	make -C ../memory
	$(CC) -o $@ $^

memt: $(MEMORY) memtest.o
	make -C ../memory
	$(CC) -o $@ $^

prst: $(OBJS) parsetest.o
	$(CC) -o $@ $^

scant: $(OBJS) scantest.o
	$(CC) -o $@ $^


keyword.c: keyword.key
	gperf -tT $^ > $@
	
y.tab.c: $(YACC)
	bison --yacc -dv $^

.c.o:
	$(CC) -c $(CFLAGS) $*.c

clean:
	rm -rf $(TARGET) *~ keyword.c *.o y.tab.h y.tab.c y.output prst memt treet
