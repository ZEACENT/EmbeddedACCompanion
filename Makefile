CURDIR=$(shell pwd)
CCTOOLS	=$(CURDIR)/YQgcc4.4_for_S5PV210/bin/arm-linux
CC		=$(CURDIR)/YQgcc4.4_for_S5PV210/bin/arm-linux-gcc
STRIP	=$(CURDIR)/YQgcc4.4_for_S5PV210/bin/arm-linux-strip
KERNEL	=$(CURDIR)/kernel
ARCH	=arm
HOST	=arm-linux-gnueabi
CFLAGS	= -L$(CURDIR)/lib -I$(CURDIR)/include -D_LINUX -std=gnu99 -lfont -lm -lfreetype -pthread \
			-L$(INSTALLDIR)/usr/local/sqlite/lib -I$(INSTALLDIR)/usr/local/sqlite/include -lsqlite3
INSTALLDIR	=$(CURDIR)/install
MAINDIR		=$(CURDIR)/main
AITALKDIR	=$(CURDIR)/aitalk
ALSADIR		=$(CURDIR)/alsa
SSHDIR		=$(CURDIR)/ssh
NTPDIR		=$(CURDIR)/ntpclient-2015
PPPDIR		=$(CURDIR)/ppp
DBDIR		=$(CURDIR)/sqlite-autoconf-3350500
IPTABLESDIR	=$(CURDIR)/iptables-1.8.7
SSLDIR		=$(CURDIR)/openssl-1.1.1k
CURLDIR		=$(CURDIR)/curl-7.76.1

MAINSRC= $(wildcard $(MAINDIR)/*.c)

.PHONY: all PREPARE embeddedACCompanion alsa ssh zlib-1.2.11 openssh ntpclient ppp rp-pppoe v2ray sqlite iptables kernel openssl curl clean cleanBuild cleanAll

all: PREPARE embeddedACCompanion alsa ssh ntpclient ppp rp-pppoe v2ray sqlite iptables curl
	cp $(MAINDIR)/embeddedACCompanion $(INSTALLDIR)/usr/local/bin
	cp $(NTPDIR)/ntpclient            $(INSTALLDIR)/usr/local/bin
	cp $(PPPDIR)/ppp-2.4.7/pppd/pppd  $(INSTALLDIR)/usr/local/bin
	cp $(CURLDIR)/install/bin/curl    $(INSTALLDIR)/usr/local/bin
	-$(STRIP) $(INSTALLDIR)/usr/local/sbin/*
	-$(STRIP) $(INSTALLDIR)/usr/local/bin/*
	-$(STRIP) $(INSTALLDIR)/usr/local/alsa/bin/*
	-$(STRIP) $(INSTALLDIR)/usr/local/sqlite/bin/*
	-$(STRIP) $(INSTALLDIR)/usr/local/iptables/sbin/*
	tar -czf install.tar.gz install

PREPARE:
		mkdir -p  $(INSTALLDIR)/usr/local/bin
		mkdir -p  $(INSTALLDIR)/usr/local/alsa
		mkdir -p  $(INSTALLDIR)/usr/local/v2ray
		mkdir -p  $(INSTALLDIR)/usr/local/sbin
		mkdir -p  $(INSTALLDIR)/usr/local/etc
		mkdir -p  $(INSTALLDIR)/usr/local/libexec
		mkdir -p  $(INSTALLDIR)/usr/local/sqlite
		mkdir -p  $(INSTALLDIR)/usr/local/iptables
		mkdir -p  $(INSTALLDIR)/var/run
		mkdir -p  $(INSTALLDIR)/var/empty

		mkdir -p $(ALSADIR)
		mkdir -p $(SSHDIR)/install
		mkdir -p $(PPPDIR)

embeddedACCompanion: $(MAINSRC) sqlite
	@cd $(MAINDIR)															\
		&& $(CC) -o $@ $(MAINSRC) $(CFLAGS)

aitalk: PREPARE
	@if [ ! -d $(AITALKDIR) ] ; then										\
		tar -xf Linux_aitalk_exp1227_bc03ec57.tar.gz;						\
	fi
	@cd $(AITALKDIR)/
	make CROSS_COMPILE=/root/EmbeddedACCompanion/YQgcc4.4_for_S5PV210/bin/arm-linux-
	cp $(AITALKDIR)/bin/*		$(INSTALLDIR)/usr/local/bin
	cp $(AITALKDIR)/include/*	$(INSTALLDIR)/usr/local/bin
	cp $(AITALKDIR)/bin/*		$(INSTALLDIR)/usr/local/bin
	cp $(AITALKDIR)/bin/*		$(INSTALLDIR)/usr/local/bin

alsa: PREPARE
	@if [ ! -d $(ALSADIR)/alsa-lib-1.2.3.2 ] ; then							\
		tar -xf $(CURDIR)/alsa-lib-1.2.3.2.tar.bz2 -C $(ALSADIR);			\
	fi
	@if [ ! -d $(ALSADIR)/alsa-utils-1.2.3 ] ; then							\
		tar -xf $(CURDIR)/alsa-utils-1.2.3.tar.bz2 -C $(ALSADIR);			\
		patch -p1 < alsa-utils-1.2.3.patch;									\
	fi
	@cd $(ALSADIR)/alsa-lib-1.2.3.2											\
		&&	CC=$(CC)														\
			./configure														\
				--host=$(HOST)												\
				--prefix=$(INSTALLDIR)/usr/local/alsa						\
		&& $(MAKE)															\
		&& $(MAKE) install
	@cd $(ALSADIR)/alsa-utils-1.2.3											\
		&&	CC=$(CC)														\
			./configure														\
				--host=$(HOST)												\
				--prefix=$(INSTALLDIR)/usr/local/alsa						\
				--with-alsa-inc-prefix=$(INSTALLDIR)/usr/local/alsa/include	\
				--with-alsa-prefix=$(INSTALLDIR)/usr/local/alsa/lib			\
				--disable-alsamixer											\
				--disable-xmlto												\
				--disable-nls												\
		&& $(MAKE)															\
		&& $(MAKE) install

ssh: openssh
	cp $(SSHDIR)/openssh-8.3p1/sshd			$(INSTALLDIR)/usr/local/sbin
	cp $(SSHDIR)/openssh-8.3p1/scp			$(INSTALLDIR)/usr/local/bin
	cp $(SSHDIR)/openssh-8.3p1/sftp			$(INSTALLDIR)/usr/local/bin
	cp $(SSHDIR)/openssh-8.3p1/ssh			$(INSTALLDIR)/usr/local/bin
	cp $(SSHDIR)/openssh-8.3p1/ssh-add		$(INSTALLDIR)/usr/local/bin
	cp $(SSHDIR)/openssh-8.3p1/ssh-agent	$(INSTALLDIR)/usr/local/bin
	cp $(SSHDIR)/openssh-8.3p1/ssh-keygen	$(INSTALLDIR)/usr/local/bin
	cp $(SSHDIR)/openssh-8.3p1/ssh-keyscan	$(INSTALLDIR)/usr/local/bin
	cp $(SSHDIR)/openssh-8.3p1/moduli		$(INSTALLDIR)/usr/local/etc
	cp $(SSHDIR)/openssh-8.3p1/ssh_config	$(INSTALLDIR)/usr/local/etc
	cp $(SSHDIR)/openssh-8.3p1/sshd_config	$(INSTALLDIR)/usr/local/etc
	cp $(SSHDIR)/openssh-8.3p1/sftp-server	$(INSTALLDIR)/usr/local/libexec
	cp $(SSHDIR)/openssh-8.3p1/ssh-keysign	$(INSTALLDIR)/usr/local/libexec

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
		&& $(MAKE) install

openssh: zlib-1.2.11
	@if [ ! -d $(SSHDIR)/openssh-8.3p1 ] ; then						\
		tar -xf openssh-8.3p1.tar.gz -C $(SSHDIR);					\
	fi
	@cd $(SSHDIR)/openssh-8.3p1										\
		&& ./configure												\
				--host=$(HOST)										\
				-with-libs											\
				--with-zlib=$(SSHDIR)/install/zlib-1.2.11			\
				--without-openssl									\
				--disable-etc-default-login							\
				LDFLAGS="-static"									\
			CC=$(CC)												\
		&& $(MAKE)

ntpclient:
	@if [ ! -d $(NTPDIR) ] ; then									\
		tar -xf ntpclient_2015_365.tar.gz;							\
	fi
	@cd $(NTPDIR)													\
		&& $(MAKE) CC=$(CC)

ppp: PREPARE
	@if [ ! -d $(PPPDIR)/ppp-2.4.7 ] ; then							\
		tar -xf ppp-2.4.7.tar.gz -C $(PPPDIR);						\
	fi
	@cd $(PPPDIR)/ppp-2.4.7											\
		&& ./configure												\
		&& $(MAKE) CC=$(CC)

rp-pppoe: PREPARE
	@if [ ! -d $(PPPDIR)/rp-pppoe-3.14 ] ; then						\
		tar -xf rp-pppoe-3.14.tar.gz -C $(PPPDIR);					\
	fi
	@cd $(PPPDIR)/rp-pppoe-3.14/src									\
		&& ./configure												\
			--host=$(HOST) 											\
			CC=$(CC)												\
		&& $(MAKE)													\
		&& $(MAKE) install exec_prefix=$(INSTALLDIR)/usr/local

v2ray: PREPARE
	@if [ ! -f v2ray-custom-arm-linux-20210508-152307.tar.gz ] ; then							\
		rm -rf /usr/local/go && tar -C /usr/local -xzf go1.16.4.linux-amd64.tar.gz;				\
		export PATH=$PATH:/usr/local/go/bin;													\
		go get -u v2ray.com/core/...;															\
		cd /root/go/pkg/mod/v2ray.com/core@v4.19.1+incompatible;								\
		wget https://raw.githubusercontent.com/v2ray/v2ray-core/master/release/user-package.sh;	\
		chmod 755 user-package.sh;																\
		./user-package.sh tgz 386 arm;															\
	fi
	tar -xf v2ray-custom-arm-linux-20210508-152307.tar.gz -C $(INSTALLDIR)/usr/local/v2ray

sqlite: PREPARE
	@if [ ! -d $(DBDIR) ] ; then									\
		tar -xf sqlite-autoconf-3350500.tar.gz;						\
	fi
	@cd $(DBDIR)													\
		&&	CC=$(CC)												\
			./configure												\
				--host=$(HOST)										\
				--prefix=$(INSTALLDIR)/usr/local/sqlite				\
		&& $(MAKE) 													\
		&& $(MAKE) install

iptables:
	@if [ ! -d $(IPTABLESDIR) ] ; then								\
		tar -xf iptables-1.8.7.tar.bz2;								\
	fi
	@cd $(IPTABLESDIR)												\
		&&	CC=$(CC)												\
			./configure												\
				--host=$(HOST)										\
				--prefix=$(INSTALLDIR)/usr/local/iptables			\
				--disable-nftables									\
				--enable-static --disable-shared					\
		&& $(MAKE) 													\
		&& $(MAKE) install

kernel:
	@if [ ! -d $(KERNEL)/Kernel_3.0.8_TQ210_for_Linux_v2.4 ] ; then						\
		tar -xf $(KERNEL)/Kernel_3.0.8_TQ210_for_Linux_v2.4.tar.xz -C $(KERNEL);		\
	fi
	@cd $(KERNEL)/Kernel_3.0.8_TQ210_for_Linux_v2.4										\
		&&	cp config_for_TQ210_Linux_v2.1_CoreB  .config								\
		&&	patch -p1 < ../Kernel_3.0.8_TQ210_for_Linux_v2.4.patch						\
		&&	ARCH=$(ARCH) CROSS_COMPILE=$(CCTOOLS)- $(MAKE) zImage 						\

openssl:
	@if [ ! -d $(SSLDIR) ] ; then									\
		tar -xf openssl-1.1.1k.tar.gz;								\
	fi
	@cd $(SSLDIR)													\
		&&	./Configure												\
				linux-armv4											\
				--cross-compile-prefix=$(CCTOOLS)-					\
				no-asm												\
				no-shared											\
				--prefix=$(SSLDIR)/install							\
		&& $(MAKE)													\
		&& $(MAKE)	install

curl: openssl
	@if [ ! -d $(CURLDIR) ] ; then									\
		tar -xf curl-7.76.1.tar.gz;									\
	fi
	@cd $(CURLDIR)													\
		&&	CC=$(CC)												\
			LDFLAGS="-static" 										\
			./configure												\
				--host=$(HOST)										\
				--prefix=$(CURLDIR)/install							\
				--with-ssl=$(SSLDIR)/install						\
				--enable-static --disable-shared					\
		&& $(MAKE)	curl_LDFLAGS=-all-static						\
		&& $(MAKE) install	curl_LDFLAGS=-all-static



clean:
	rm -rf $(AITALKDIR)
	rm -rf $(ALSADIR)
	rm -rf $(SSHDIR)
	rm -rf $(NTPDIR)
	rm -rf $(PPPDIR)
	rm -rf $(DBDIR)
	rm -rf $(IPTABLESDIR)
	rm -rf $(KERNEL)/Kernel_3.0.8_TQ210_for_Linux_v2.4
	rm -rf $(SSLDIR)
	rm -rf $(CURLDIR)

cleanBuild:
	rm -rf $(INSTALLDIR)*

cleanAll: clean cleanBuild

