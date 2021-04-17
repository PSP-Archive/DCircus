TARGET = dcircus_psp
OBJS = gpdata.o dcircus.o game.o gameover.o highscore.o rand.o setup.o title.o

INCDIR = 
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBS=-lSDL_mixer -lSDL_image -lSDL -lpspaudiolib -lpspaudio -lpspgum -lpspgu -lpsphprm -lpng -ljpeg -lz -lc -lm -lgcc
LIBDIR =
LDFLAGS =

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = dcircus_psp

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

