CFLAGS = -O3 -g
LINKER = -Xlinker -Ttext -Xlinker 80140000

PROG = main.exe
OBJS = main.o font.o controller.o keyboard.o

all: $(PROG)

$(PROG): $(OBJS)
	$(CC) $(LINKER) -o $@ $? -lps

main.o: main.c font.h
	$(CC) $(CFLAGS) -c main.c

font.o: font.c font.h
	$(CC) $(CFLAGS) -c font.c

keyboard.o: keyboard.c keyboard.h
	$(CC) $(CFLAGS) -c keyboard.c

controller.o: controller.h controller.c
	$(CC) $(CFLAGS) -c controller.c

rebuild: 
	make clean
	echo ---------- building ------
	make all 

psx.exe: $(PROG) main.exe 
	yarexe AUTO

clean:
	$(RM) $(PROG)
	$(RM) $(OBJS)