CURDIR=$(shell pwd)
CC=$(CURDIR)/YQgcc4.4_for_S5PV210/bin/arm-linux-gcc
HOST=arm-linux-gnueabihf
HFCC=arm-linux-gnueabihf-gcc
CFLAGS= -L$(CURDIR)/lib -I$(CURDIR)/include -D_LINUX -std=gnu99 -lfont -lm -lfreetype -pthread 
INSTALLDIR=$(CURDIR)/install
MAINDIR=$(CURDIR)/main
DHCPDIR=$(CURDIR)/dhcp-4.4.2
SSHDIR=$(CURDIR)/ssh

MAINSRC= $(wildcard $(MAINDIR)/*.c)
MAINOBJ= $(patsubst %.c, %.o, ${MAINSRC})

.PHONY: all
all: PREPARE EmbeddedACCompanion alsa dhclient ssh
	mv $(MAINDIR)/EmbeddedACCompanion $(INSTALLDIR)/usr/local/bin

PREPARE:
	@if [ ! -d $(INSTALLDIR) ] ; then						\
		mkdir -p  $(INSTALLDIR);							\
		mkdir -p  $(INSTALLDIR)/usr/local/bin;				\
		mkdir -p  $(INSTALLDIR)/usr/local/sbin;				\
		mkdir -p  $(INSTALLDIR)/usr/local/etc;				\
		mkdir -p  $(INSTALLDIR)/usr/local/libexec;			\
		mkdir -p  $(INSTALLDIR)/var/run;					\
		mkdir -p  $(INSTALLDIR)/var/empty;					\
	fi
	@if [ ! -d $(SSHDIR) ] ; then							\
		mkdir -p $(SSHDIR);									\
		mkdir -p $(SSHDIR)/install;							\
	fi

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

EmbeddedACCompanion: $(MAINOBJ)
	@cd $(MAINDIR)											\
		&& $(CC) -o $@ $^ $(CFLAGS)

alsa: PREPARE
	tar -xf $(CURDIR)/alsa.tar.gz -C $(INSTALLDIR)

dhclient: PREPARE
	@if [ ! -d $(DHCPDIR) ] ; then						\
		tar -xf $(DHCPDIR).tar;							\
	fi
	@cd $(DHCPDIR)											\
		&& ./configure										\
		&& patch -f -p1 < ../dhcp-4.4.2.patch				\
		&& $(MAKE) "CC=$(HFCC) -static"
	mv $(DHCPDIR)/client/dhclient $(INSTALLDIR)/usr/local/bin

.PHONY: ssh
ssh: PREPARE zlib-1.2.11 openssl-1.1.1k openssh
	mv $(SSHDIR)/openssh/sshd			$(INSTALLDIR)/usr/local/sbin
	
	mv $(SSHDIR)/openssh/scp			$(INSTALLDIR)/usr/local/bin
	mv $(SSHDIR)/openssh/sftp			$(INSTALLDIR)/usr/local/bin
	mv $(SSHDIR)/openssh/ssh			$(INSTALLDIR)/usr/local/bin
	mv $(SSHDIR)/openssh/ssh-add		$(INSTALLDIR)/usr/local/bin
	mv $(SSHDIR)/openssh/ssh-agent		$(INSTALLDIR)/usr/local/bin
	mv $(SSHDIR)/openssh/ssh-keygen		$(INSTALLDIR)/usr/local/bin
	mv $(SSHDIR)/openssh/ssh-keyscan	$(INSTALLDIR)/usr/local/bin

	mv $(SSHDIR)/openssh/moduli			$(INSTALLDIR)/usr/local/etc
	mv $(SSHDIR)/openssh/ssh_config		$(INSTALLDIR)/usr/local/etc
	mv $(SSHDIR)/openssh/sshd_config	$(INSTALLDIR)/usr/local/etc

	mv $(SSHDIR)/openssh/sftp-server	$(INSTALLDIR)/usr/local/libexec
	mv $(SSHDIR)/openssh/ssh-keysign	$(INSTALLDIR)/usr/local/libexec

zlib-1.2.11: PREPARE
	@if [ ! -d $(SSHDIR)/zlib-1.2.11 ] ; then					\
		tar -xf zlib-1.2.11.tar.gz -C $(SSHDIR);				\
	fi
	@cd $(SSHDIR)/zlib-1.2.11									\
		&& ./configure --prefix=$(SSHDIR)/install/zlib-1.2.11	\
		&& patch -f -p1 < $(CURDIR)/zlib-1.2.11.patch			\
		&& $(MAKE)												\
		&& $(MAKE) install

# openssl-1.0.1t: PREPARE
# 	@if [ ! -d $(SSHDIR)/openssl-1.0.1t ] ; then				\
# 		tar -xf openssl-1.0.1t.tar.gz -C $(SSHDIR);				\
# 	fi
# 	@cd $(SSHDIR)/openssl-1.0.1t														\
# 		&& ./Configure --prefix=$(SSHDIR)/install/openssl-1.0.1t os/compiler:$(HFCC)	\
# 		&& $(MAKE)																		\
# 		&& $(MAKE) install
openssl-1.1.1k: PREPARE
	@if [ ! -d $(SSHDIR)/openssl-1.1.1k ] ; then				\
		tar -xf openssl-1.1.1k.tar.gz -C $(SSHDIR);				\
	fi
	@cd $(SSHDIR)/openssl-1.1.1k														\
		&& ./config no-asm shared --prefix=$(SSHDIR)/install/openssl-1.1.1k shared		\
		&& patch -f -p1 < $(CURDIR)/openssl-1.1.1k.patch								\
		&& $(MAKE)																		\
		&& $(MAKE) install

openssh: PREPARE
	@if [ ! -d $(SSHDIR)/openssh ] ; then							\
		tar -xf openssh-SNAP-20210331.tar.gz -C $(SSHDIR);			\
	fi
	@cd $(SSHDIR)/openssh											\
		&& ./configure												\
			--host=$(HOST) 											\
			--with-libs 											\
			--with-zlib=$(SSHDIR)/install/zlib-1.2.11 				\
			--with-ssl-dir=$(SSHDIR)/install/openssl-1.1.1k 		\
			--disable-etc-default-login 							\
			CC=$(HFCC) 												\
			AR=$(HOST)-ar											\
		&& $(MAKE)
	

.PHONY: clean
clean:
	rm -f $(MAINOBJ)
	rm -rf $(DHCPDIR)
	rm -rf $(SSHDIR)

.PHONY: cleanBuild
cleanBuild:
	rm -rf $(INSTALLDIR)

.PHONY: cleanAll
cleanAll: clean cleanBuild
