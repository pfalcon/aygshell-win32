ARCH=i686-w64-mingw32
CC=$(ARCH)-gcc
CXX=$(ARCH)-g++
RANLIB=$(ARCH)-ranlib
WINDRES=$(ARCH)-windres


INC=-I$(AYGSHELL_WIN32_PATH)/aygshell "-I$(WIN32XX_PATH)/include"
DEFINES=-DUNICODE -D_UNICODE
CFLAGS=$(DEFINES) $(INC)
CXXFLAGS=$(CFLAGS) -fno-rtti -g
LDFLAGS=-static -municode
#LDLIBS=-laygshell -lcommctrl
LDLIBS=-lgdi32 -lcomctl32

OBJS=\
aygshell.o \


all: libaygshell.a

libaygshell.a: $(OBJS)
	$(AR) r $@ $^
	$(RANLIB) $@

clean:
	rm -f *.o
