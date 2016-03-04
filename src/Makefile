CROSS			=arm-none-linux-gnueabi-
CC				=$(CROSS)gcc
TARGET			=gather_V0
BIN				=../bin/$(TARGET)
SRC				=main.c sysinit.c uart_set.c commap.c queue.c sysfuctions.c xml.c rs485up.c
INCLUDEPATH		=-I ../include -I ../include/libxml2
OBJ				=-o $(BIN)
CFLAGS			=-g -Wall 
LIBDIR			=-L ../lib
LIBS			=-lxml2  -lsqlite3 -lcrypto -lpthread
NFSROOT			=/mcuzone/nfsroot
CP				=cp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(OBJ) $(CFLAGS) $(SRC) $(INCLUDEPATH) $(LIBDIR) $(LIBS)
	$(CP) $(BIN) $(NFSROOT)
clean:
	@rm -vf $(BIN) *.o *~
