CROSS = arm-none-linux-gnueabi-
C_RELY = main.c sysinit.c uart_set.c commap.c queue.c sysfuctions.c xml.c rs485up.c readallmeters.c read_heatmeter.c \
		 meter_table.c bsp.c uart_mbus_down485.c uart_gprs.c 

all: gather_V0

gather_V0: $(C_RELY)
	$(CROSS)gcc -Wall -o gather_V0 -g $(C_RELY) \
	 -lpthread -I ../include -L ../include/sqlite/lib -lsqlite3
	 cp ./gather_V0 /mcuzone/nfsroot/
	
#	 -lpthread -I ./include/libxml2 -I ./openssl/include -L ./arm_lib -lxml2  -L ./sqlite/lib -L ./openssl/lib -lsqlite3 -lssl -lcrypto


clean:
	@rm -vf gather_V0  *.o *~

	