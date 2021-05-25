### For running cross-compiler:
    apt install lib32z1-dev
    apt install libncurses5-dev

## Install

ssh-keygen [*All default*]

vi ./usr/local/etc/sshd_config
```
PermitRootLogin yes
PubkeyAuthentication yes
AuthorizedKeysFile      /.ssh/authorized_keys
PasswordAuthentication yes
PermitEmptyPasswords yes
```

### vi /etc/init.d/rcS
```
PATH=/sbin:/bin:/usr/sbin:/usr/bin:/usr/local/sbin:/usr/local/bin
PATH=$PATH:/usr/local/alsa
PATH=$PATH:/usr/local/v2ray
PATH=$PATH:/usr/local/sqlite/bin
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/sqlite/lib

/sbin/udhcpc -q > /main.log &
/usr/local/bin/ntpclient -s -d -c 1 -i 5 -h ntp.aliyun.com > /main.log &
/usr/local/sbin/sshd -h /.ssh/id_ed25519 > /main.log &
/usr/local/bin/embeddedACCompanion > /main.log &
nohup /upgrade_zv.sh > /main.log &

```
### vi /etc/profile
```
PATH=/sbin:/bin:/usr/sbin:/usr/bin:/usr/local/sbin:/usr/local/bin
PATH=$PATH:/usr/local/alsa/bin
PATH=$PATH:/usr/local/v2ray
PATH=$PATH:/usr/local/sqlite/bin
```

### vi /etc/passwd
```
sshd:x:74:74:Privilege-separated SSH:/var/empty/sshd:/sbin/nologin
```

### pppoe
```
scp ./ppp/rp-pppoe-3.14/configs/* root@192.168.3.44:/etc/ppp

vi /usr/local/sbin/pppoe-setup
# From AUTOCONF
prefix=/usr/local
exec_prefix=${prefix}

# Paths to programs
IFCONFIG=/sbin/ifconfig
PPPD=/usr/local/bin/pppd
PPPOE=${exec_prefix}/sbin/pppoe

[root@GEC210 /]# pppoe-setup 
Welcome to the RP-PPPoE client setup.  First, I will run
some checks on your system to make sure the PPPoE client is installed
properly...

Looks good!  Now, please enter some information:

USER NAME

>>> Enter your PPPoE user name (default bxxxnxnx@sympatico.ca): abcd

INTERFACE

>>> Enter the Ethernet interface connected to the DSL modem
For Solaris, this is likely to be something like /dev/hme0.
For Linux, it will be ethn, where 'n' is a number.
(default eth0): 1234

Do you want the link to come up on demand, or stay up continuously?
If you want it to come up on demand, enter the idle time in seconds
after which the link should be dropped.  If you want the link to
stay up permanently, enter 'no' (two letters, lower-case.)
NOTE: Demand-activated links do not interact well with dynamic IP
addresses.  You may have some problems with demand-activated links.
>>> Enter the demand value (default no): 

DNS

Please enter the IP address of your ISP's primary DNS server.
If your ISP claims that 'the server will provide DNS addresses',
enter 'server' (all lower-case) here.
If you just press enter, I will assume you know what you are
doing and not modify your DNS setup.
>>> Enter the DNS information here: 114.114.114.114
Please enter the IP address of your ISP's secondary DNS server.
If you just press enter, I will assume there is only one DNS server.
>>> Enter the secondary DNS server address here: 8.8.8.8

PASSWORD

>>> Please enter your PPPoE password:    
>>> Please re-enter your PPPoE password: 

FIREWALLING

Please choose the firewall rules to use.  Note that these rules are
very basic.  You are strongly encouraged to use a more sophisticated
firewall setup; however, these will provide basic security.  If you
are running any servers on your machine, you must choose 'NONE' and
set up firewalling yourself.  Otherwise, the firewall rules will deny
access to all standard servers like Web, e-mail, ftp, etc.  If you
are using SSH, the rules will block outgoing SSH connections which
allocate a privileged source port.

The firewall choices are:
0 - NONE: This script will not set any firewall rules.  You are responsible
          for ensuring the security of your machine.  You are STRONGLY
          recommended to use some kind of firewall rules.
1 - STANDALONE: Appropriate for a basic stand-alone web-surfing workstation
2 - MASQUERADE: Appropriate for a machine acting as an Internet gateway
                for a LAN
>>> Choose a type of firewall (0-2): 0

** Summary of what you entered **

Ethernet Interface: 1234
User name:          abcd
Activate-on-demand: No
Primary DNS:        114.114.114.114
Secondary DNS:      8.8.8.8
Firewalling:        NONE

>>> Accept these settings and adjust configuration files (y/n)? y
Adjusting /etc/ppp/pppoe.conf
Adjusting /etc/resolv.conf
  (But first backing it up to /etc/resolv.conf-bak)
Adjusting /etc/ppp/pap-secrets and /etc/ppp/chap-secrets
  (But first backing it up to /etc/ppp/pap-secrets-bak)



Congratulations, it should be all set up!

Type 'pppoe-start' to bring up your PPPoE link and 'pppoe-stop' to bring
it down.  Type 'pppoe-status' to see the link status.

[root@GEC210 /]# pppoe-start
........^C
```

### v2ray
```
//download go
rm -rf /usr/local/go && tar -C /usr/local -xzf go1.16.4.linux-amd64.tar.gz
export PATH=$PATH:/usr/local/go/bin
go version
//get v2ray src, use proxy
go get -u v2ray.com/core/...
cd /root/go/pkg/mod/v2ray.com/core@v4.19.1+incompatible
wget https://raw.githubusercontent.com/v2ray/v2ray-core/master/release/user-package.sh
chmod 755 user-package.sh
./user-package.sh tgz 386 arm
cp v2ray-custom-arm-linux-20210508-152307.tar.gz ~/EmbeddedACCompanion/
```

### alsa
```
mkdir /dev/snd
cd /dev/snd
mknod dsp c 14 3
mknod audio c 14 4
mknod mixer c 14 0
mknod controlCO c 116 0
mknod seq c 116 1
mknod pcmCODOc c 116 24
mknod pcmCOD0p c 116 16
mknod pcmCOD1c c 116 25
mknod pcmCOD1p c 116 17
mknod timer c 116 33

接下来就可以录音了：
arecord -d3 -c1 -r16000 -twav -fS16_LE bbb.wav
-d：录音时长
-c：音轨数量
-r：采样频率
-t：保存类型
```