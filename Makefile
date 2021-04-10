CURDIR=$(shell pwd)
CC=$(CURDIR)/YQgcc4.4_for_S5PV210/bin/arm-linux-gcc
DHCPCC=arm-linux-gnueabihf-gcc
CFLAGS= -L$(CURDIR)/lib -I$(CURDIR)/include -D_LINUX -std=gnu99 -lfont -lm -lfreetype -pthread 
MAINDIR=$(CURDIR)/main
BUILDDIR=$(CURDIR)/build
DHCPDIR=$(CURDIR)/dhcp-4.4.2

MAINSRC= $(wildcard $(MAINDIR)/*.c)
MAINOBJ= $(patsubst %.c, %.o, ${MAINSRC})

.PHONY: all
all: PREPARE EmbeddedACCompanion alsa dhclient
	mv $(MAINDIR)/EmbeddedACCompanion	$(BUILDDIR)/
	mv $(DHCPDIR)/client/dhclient 			$(BUILDDIR)/

PREPARE:
	@if [ ! -d $(BUILDDIR) ] ; then						\
		mkdir  $(BUILDDIR);								\
	fi

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

EmbeddedACCompanion: $(MAINOBJ)
	@cd $(MAINDIR)											\
		&& $(CC) -o $@ $^ $(CFLAGS)

alsa:
	tar -xf $(CURDIR)/alsa.tar.gz -C $(BUILDDIR)

dhclient:
	@if [ ! -d $(DHCPDIR) ] ; then						\
		tar -xf $(DHCPDIR).tar;							\
	fi
	@cd $(DHCPDIR)											\
		&& ./configure										\
		&& patch -f -p1 < ../dhcp-4.4.2.patch				\
		&& $(MAKE) "CC=$(DHCPCC) -static"

.PHONY: clean
clean:
	rm -f $(MAINOBJ)
	rm -rf $(DHCPDIR)

.PHONY: cleanBuild
cleanBuild:
	rm -rf $(BUILDDIR)

.PHONY: cleanAll
cleanAll: clean cleanBuild
	

#all: edp

#CC=gcc
#CFLAGS= -I. -D_LINUX -lm  -pthread
#MAINOBJ = cJSON.o EdpKit.o Main.o ConnectOneNet.o

# 如果需要加密功能且系统内已经安装openssl，
# 取消以下两行注释
#CFLAGS+=-D_ENCRYPT -lcrypto
#MAINOBJ += Openssl.o
# gcc *.c -o edp  -I. -D_LINUX -lm  -pthread

#%.o: %.c
#        $(CC) -c -o $@ $< $(CFLAGS)

#edp: $(MAINOBJ)
#        $(CC) -o $@ $^ $(CFLAGS)

#clean:
#        rm -f edp $(MAINOBJ)


#arm-linux-gcc  test.c  -o font  -L./ -lfont  -lm
#arm-linux-gcc -std=c99 -lfreetype -lpthread -o FnPj *.c #拷贝头和库文件
#arm-linux-gcc *.c -o Finals -L./ -I. -D_LINUX -std=gnu99 -lfont -lm -lfreetype -pthread

