CC = /usr/bin/gcc

TARGET = svm
CFLAGS = -c -g -DDEBUG

MEMORY = ../memory/memory.o ../memory/storage.o
OBJS = disasm.o opinfo.o svm.o

all: $(TARGET)

$(TARGET): $(MEMORY) $(OBJS)
	make -C ../memory
	$(CC) -o $@ $^

.c.o:
	$(CC) $(CFLAGS) $*.c

clean:
	rm -rf $(TARGET) *.o *~