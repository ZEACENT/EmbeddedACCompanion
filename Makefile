.PHONY: all
all: EmbeddedACCompanion.exe dhclient

CC=arm-linux-gcc
CFLAGS= -L./lib -I./include -D_LINUX -std=gnu99 -lfont -lm -lfreetype -pthread 
SRC= $(wildcard *.c)
CLIENT_OBJ= $(patsubst %.c, %.o, ${SRC})
CURDIR=$(shell pwd)
BUILDDIR=$(CURDIR)/build
DHCPDIR=$(CURDIR)/dhcp-4.4.2

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

EmbeddedACCompanion.exe: $(CLIENT_OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
	@if [ ! -d $(BUILDDIR) ] ; then						\
		mkdir  $(BUILDDIR);								\
	fi
	mv $@ $(BUILDDIR)/

dhclient:
	@if [ ! -d $(DHCPDIR) ] ; then						\
		tar -xf $(DHCPDIR).tar;							\
	fi
	@cd $(DHCPDIR) && ./configure --host=arm-linux		\
						--with-randomdev=no				\
						--prefix=$(BUILDDIR)			\
						ac_cv_file__dev_random=yes		\
						CC=$(CC)						\
						BUILD_CC=gcc					\
	&& $(MAKE)											\
	&& mv $(DHCPDIR)/client/dhclient $(BUILDDIR)/

.PHONY: clean
clean:
	rm -f $(CLIENT_OBJ)
	rm -rf $(DHCPDIR)

.PHONY: cleanBuild
cleanBuild:
	rm -rf $(BUILDDIR)

.PHONY: cleanAll
cleanAll: clean cleanBuild
	

#all: edp

#CC=gcc
#CFLAGS= -I. -D_LINUX -lm  -pthread
#CLIENT_OBJ = cJSON.o EdpKit.o Main.o ConnectOneNet.o

# 如果需要加密功能且系统内已经安装openssl，
# 取消以下两行注释
#CFLAGS+=-D_ENCRYPT -lcrypto
#CLIENT_OBJ += Openssl.o
# gcc *.c -o edp  -I. -D_LINUX -lm  -pthread

#%.o: %.c
#        $(CC) -c -o $@ $< $(CFLAGS)

#edp: $(CLIENT_OBJ)
#        $(CC) -o $@ $^ $(CFLAGS)

#clean:
#        rm -f edp $(CLIENT_OBJ)


#arm-linux-gcc  test.c  -o font  -L./ -lfont  -lm
#arm-linux-gcc -std=c99 -lfreetype -lpthread -o FnPj *.c #拷贝头和库文件
#arm-linux-gcc *.c -o Finals -L./ -I. -D_LINUX -std=gnu99 -lfont -lm -lfreetype -pthread

