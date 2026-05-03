ISODIR_UNIX = C:/psx3/sbin/yariso/NYROM
ISODIR_WIN  = C:\psx3\sbin\yariso\NYROM

CFLAGS = -O3 -g
LINKER = -Xlinker -Ttext -Xlinker 80140000

PROG = main.exe
OBJS = main.o pad.o

all: $(PROG)

$(PROG): $(OBJS)
	$(CC) $(LINKER) -o $@ $? -lps

main.o: main.c pad.h
	$(CC) $(CFLAGS) -c main.c

pad.o: pad.h pad.c

rebuild: 
	make clean
	echo ---------- building ------
	make all 

psx.exe: $(PROG) main.exe 
	yarexe AUTO

clean:
	$(RM) $(PROG)
	$(RM) $(OBJS)

release:
	make clean
	echo ---------- RELEASE ------
	make CFLAGS=" -O3 -Werror -DNETYAROZE -D_RELEASE_"  all 
	yarexe AUTO 
	make psx.iso

psx.iso: psx.exe
	rm -f psx.iso
	
	@echo Cleaning NYROM folder
	@if exist "$(ISODIR_WIN)" del /Q "$(ISODIR_WIN)\*.*"
	@if exist "$(ISODIR_WIN)\DATA" rmdir /S /Q "$(ISODIR_WIN)\DATA"
	
	@echo Copy executable
	cp -f psx.exe $(ISODIR_UNIX)/PSX.EXE

	@echo Copy assets from local data folder
	xcopy /E /Y /I data "$(ISODIR_WIN)\DATA\"

	@echo Build ISO
	yariso SCEI NY

	@echo Copy finished ISO back
	cp -f C:/psx3/sbin/yariso/psx.iso .
