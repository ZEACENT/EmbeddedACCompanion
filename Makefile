CURDIR=$(shell pwd)
CCTOOLS	=$(CURDIR)/YQgcc4.4_for_S5PV210/bin/arm-linux
CC		=$(CURDIR)/YQgcc4.4_for_S5PV210/bin/arm-linux-gcc
STRIP	=$(CURDIR)/YQgcc4.4_for_S5PV210/bin/arm-linux-strip
HOST	=arm-linux-gnueabi
CFLAGS	= -L$(CURDIR)/lib -I$(CURDIR)/include -D_LINUX -std=gnu99 -lfont -lm -lfreetype -pthread 
INSTALLDIR	=$(CURDIR)/install
MAINDIR		=$(CURDIR)/main
SSHDIR		=$(CURDIR)/ssh
NTPDIR		=$(CURDIR)/ntpclient-2015

MAINSRC= $(wildcard $(MAINDIR)/*.c)
MAINOBJ= $(patsubst %.c, %.o, ${MAINSRC})

.PHONY: all PREPARE embeddedACCompanion alsa ssh zlib-1.2.11 openssh ntpclient clean cleanBuild cleanAll

all: PREPARE embeddedACCompanion alsa ssh ntpclient
	mv $(MAINDIR)/embeddedACCompanion $(INSTALLDIR)/usr/local/bin
	mv $(NTPDIR)/ntpclient $(INSTALLDIR)/usr/local/bin
	$(STRIP) $(INSTALLDIR)/usr/local/sbin/*
	$(STRIP) $(INSTALLDIR)/usr/local/bin/*
	tar -czf install.tar.gz install

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

embeddedACCompanion: $(MAINOBJ)
	@cd $(MAINDIR)											\
		&& $(CC) -o $@ $^ $(CFLAGS)

alsa: PREPARE
	tar -xf $(CURDIR)/alsa.tar.gz -C $(INSTALLDIR)

ssh: PREPARE openssh
	mv $(SSHDIR)/openssh-8.3p1/sshd			$(INSTALLDIR)/usr/local/sbin
	mv $(SSHDIR)/openssh-8.3p1/scp			$(INSTALLDIR)/usr/local/bin
	mv $(SSHDIR)/openssh-8.3p1/sftp			$(INSTALLDIR)/usr/local/bin
	mv $(SSHDIR)/openssh-8.3p1/ssh			$(INSTALLDIR)/usr/local/bin
	mv $(SSHDIR)/openssh-8.3p1/ssh-add		$(INSTALLDIR)/usr/local/bin
	mv $(SSHDIR)/openssh-8.3p1/ssh-agent	$(INSTALLDIR)/usr/local/bin
	mv $(SSHDIR)/openssh-8.3p1/ssh-keygen	$(INSTALLDIR)/usr/local/bin
	mv $(SSHDIR)/openssh-8.3p1/ssh-keyscan	$(INSTALLDIR)/usr/local/bin
	mv $(SSHDIR)/openssh-8.3p1/moduli		$(INSTALLDIR)/usr/local/etc
	mv $(SSHDIR)/openssh-8.3p1/ssh_config	$(INSTALLDIR)/usr/local/etc
	mv $(SSHDIR)/openssh-8.3p1/sshd_config	$(INSTALLDIR)/usr/local/etc
	mv $(SSHDIR)/openssh-8.3p1/sftp-server	$(INSTALLDIR)/usr/local/libexec
	mv $(SSHDIR)/openssh-8.3p1/ssh-keysign	$(INSTALLDIR)/usr/local/libexec

zlib-1.2.11: PREPARE
	@if [ ! -d $(SSHDIR)/zlib-1.2.11 ] ; then					\
		tar -xf zlib-1.2.11.tar.gz -C $(SSHDIR);				\
	fi
	@cd $(SSHDIR)/zlib-1.2.11									\
		&& prefix=$(SSHDIR)/install/zlib-1.2.11					\
			CC=$(CC)											\
			CFLAGS="-static -fPIC"								\
			./configure											\
		&& $(MAKE)												\
		&& $(MAKE) install										\

openssh: PREPARE zlib-1.2.11
	@if [ ! -d $(SSHDIR)/openssh-8.3p1 ] ; then						\
		tar -xf openssh-8.3p1.tar.gz -C $(SSHDIR);					\
	fi
	@cd $(SSHDIR)/openssh-8.3p1										\
		&& ./configure												\
			--host=$(HOST) 											\
			-with-libs 												\
			--with-zlib=$(SSHDIR)/install/zlib-1.2.11 				\
			--without-openssl 										\
			--disable-etc-default-login 							\
			LDFLAGS="-static -pthread" 								\
			CC=$(CC)												\
		&& $(MAKE)

ntpclient:
	@if [ ! -d $(NTPDIR) ] ; then									\
		tar -xf ntpclient_2015_365.tar.gz;							\
	fi
	@cd $(NTPDIR)													\
		&& $(MAKE) CC=$(CC)
	

clean:
	rm -rf $(MAINOBJ)
	rm -rf $(SSHDIR)
	rm -rf $(NTPDIR)

cleanBuild:
	rm -rf $(INSTALLDIR)*

cleanAll: clean cleanBuild

