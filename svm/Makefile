CC = /usr/bin/gcc

TARGET = svm
CFLAGS = -c -g -DDEBUG

OBJS = svm.o opinfo.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS)

.c.o:
	$(CC) $(CFLAGS) $*.c

clean:
	rm -rf $(TARGET) *.o *~ disasm svm

