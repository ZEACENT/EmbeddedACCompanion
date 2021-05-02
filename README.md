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
udhcpc -q
/usr/local/sbin/sshd -h /.ssh/id_ed25519
```

### vi /etc/profile
```
export PATH=$PATH:/usr/local/sbin
ntpclient -s -d -c 1 -i 5 -h ntp.aliyun.com 
```

### vi /etc/passwd
```
sshd:x:74:74:Privilege-separated SSH:/var/empty/sshd:/sbin/nologin
```