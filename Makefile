CURDIR=$(shell pwd)
CCTOOLS	=$(CURDIR)/YQgcc4.4_for_S5PV210/bin/arm-linux
CC		=$(CURDIR)/YQgcc4.4_for_S5PV210/bin/arm-linux-gcc
STRIP	=$(CURDIR)/YQgcc4.4_for_S5PV210/bin/arm-linux-strip
HOST	=arm-linux-gnueabi
CFLAGS	= -L$(CURDIR)/lib -I$(CURDIR)/include -D_LINUX -std=gnu99 -lfont -lm -lfreetype -pthread 
INSTALLDIR	=$(CURDIR)/install
MAINDIR		=$(CURDIR)/main
SSHDIR		=$(CURDIR)/ssh

MAINSRC= $(wildcard $(MAINDIR)/*.c)
MAINOBJ= $(patsubst %.c, %.o, ${MAINSRC})

.PHONY: all PREPARE embeddedACCompanion alsa ssh zlib-1.2.11 openssh clean cleanBuild cleanAll

all: PREPARE embeddedACCompanion alsa ssh
	mv $(MAINDIR)/embeddedACCompanion $(INSTALLDIR)/usr/local/bin

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
	

clean:
	rm -rf $(MAINOBJ)
	rm -rf $(SSHDIR)

cleanBuild:
	rm -rf $(INSTALLDIR)

cleanAll: clean cleanBuild

