CROSS			=arm-none-linux-gnueabi-
CC				=$(CROSS)gcc
TARGET			=gather_V0
BIN				=../bin/$(TARGET)
DB				=../db/gatherdb.db
SRC				=main.c sysinit.c uart_set.c commap.c queue.c \
					sysfuctions.c xml.c rs485up.c readallmeters.c \
					read_heatmeter.c elect.c water.c meter_table.c \
					bsp.c uart_mbus_down485.c uart_gprs.c db.c gprs.c
RCS				=../sh/rcS
SCRIPT			=../sh/runapp.sh
INCLUDEPATH		=-I ../include -I ../include/libxml2
OBJ				=-o $(BIN)
CFLAGS			=-g -Wall 
LIBDIR			=-L ../lib
LIBS			= -lsqlite3 -lpthread -lxml2
NFSROOT			=/mcuzone/nfsroot
CP				=cp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(OBJ) $(CFLAGS) $(SRC) $(INCLUDEPATH) $(LIBDIR) $(LIBS)
	$(CP) $(BIN) $(NFSROOT)
	$(CP) $(BIN) ~/nfsmnt
	$(CP) $(SCRIPT) $(NFSROOT)
	$(CP) $(SCRIPT) ~/nfsmnt
	$(CP) $(SCRIPT) /mcuzone/nfsroot/minigui/app
	$(CP) $(BIN) /mcuzone/nfsroot/minigui/app
	$(CP) $(DB) /mcuzone/nfsroot/minigui/app
	$(CP) $(RCS) /mcuzone/nfsroot/minigui/etc/init.d
	sudo mkfs.ubifs -r $(NFSROOT)/minigui -m 2048 -e 126976 -c 2047 -o $(NFSROOT)/ubi.in
	sudo ubinize -o $(NFSROOT)/ubi.img -m 2048 -p 128KiB -s 2048 $(NFSROOT)/ubinize200.cfg
clean:
	@rm -vf $(BIN) *.o *~
