CC = /usr/bin/gcc

TARGET = disasm
CFLAGS = -c -g -DDEBUG

OBJS = disasm.o opinfo.o dmain.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS)

.c.o:
	$(CC) $(CFLAGS) $*.c

clean:
	rm -rf $(TARGET) *.o *~