TARGET = parsetest
CC = /usr/bin/gcc
CFLAGS = -g -DDEBUG
OBJS = scanner.o keyword.o
EXEC = parser.o

all: $(OBJS) $(EXEC)
	$(CC) -o $(TARGET) $^


keyword.c: keyword.key
	gperf -tT $^ > $@

.c.o:
	$(CC) -c $(CFLAGS) $*.c

clean:
	rm -rf $(TARGET) *~ keyword.c *.o
