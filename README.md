### For running cross-compiler:
    apt install lib32z1-dev


## BUILD


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
udhcpc -q &
ntpclient -s -d -c 1 -i 5 -h ntp.aliyun.com &
/usr/local/sbin/sshd -h /.ssh/id_ed25519 &
embeddedACCompanion &
```

### vi /etc/passwd
```
sshd:x:74:74:Privilege-separated SSH:/var/empty/sshd:/sbin/nologin
```

### pppoe
```
pppoe-setup
pppoe-start
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
wget https://raw.githubusercontent.com/v2ray/v2ray-core/master/release/user-package.sh) 
chmod 755 user-package.sh
./user-package.sh tgz 386 arm
cp v2ray-custom-arm-linux-20210508-152307.tar.gz ~/EmbeddedACCompanion/
```