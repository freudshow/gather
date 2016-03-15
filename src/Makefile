CROSS			=arm-none-linux-gnueabi-
CC				=$(CROSS)gcc
TARGET			=gather_V0
BIN				=../bin/$(TARGET)
SRC				=main.c sysinit.c uart_set.c commap.c queue.c sysfuctions.c xml.c rs485up.c readallmeters.c read_heatmeter.c \
		 			meter_table.c bsp.c uart_mbus_down485.c uart_gprs.c db.c gprs.c 
INCLUDEPATH		=-I ../include -I ../include/libxml2
OBJ				=-o $(BIN)
CFLAGS			=-g -Wall 
LIBDIR			=-L ../lib
LIBS			= -lsqlite3 -lpthread
NFSROOT			=/mcuzone/nfsroot
CP				=cp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(OBJ) $(CFLAGS) $(SRC) $(INCLUDEPATH) $(LIBDIR) $(LIBS)
	$(CP) $(BIN) $(NFSROOT)
clean:
	@rm -vf $(BIN) *.o *~
