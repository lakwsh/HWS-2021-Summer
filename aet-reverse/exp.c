#include <stdio.h>
#include <stdint.h>

char key[0x10] = {0, 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0};
unsigned char flag[] = {0x42, 0xC7, 0xCA, 0x40, 0xC1, 0x75, 0x16, 0xEF, 0xE7, 0x37, 0x6E, 0x69, 0x1B, 0x0B, 0x0F, 0x78, 0xDF, 0xE0, 0xE0, 0x7B, 0x5F, 0x50, 0x57, 0x05, 0xF4, 0x73, 0xD2, 0x35, 0x47, 0xD5, 0x6C, 0x5A};

void decrypt (unsigned char *v) {
	uint32_t sum = 0xC6EF3720;
	uint32_t delta = 0x9e3779b9;
	uint32_t k0 = (key[2]<<8)|(key[1]<<16)|(key[0]<<24)|key[3];
	uint32_t k1 = (key[6]<<8)|(key[5]<<16)|(key[4]<<24)|key[7];
	uint32_t k2 = (key[10]<<8)|(key[9]<<16)|(key[8]<<24)|key[11];
	uint32_t k3 = (key[14]<<8)|(key[13]<<16)|(key[12]<<24)|key[15];
	uint32_t v0 = (v[2]<<8)|(v[1]<<16)|(v[0]<<24)|v[3];
	uint32_t v1 = (v[6]<<8)|(v[5]<<16)|(v[4]<<24)|v[7];
	for (int i=0; i<32; i++) {
		v1 -= ((v0<<4)+k2)^(v0+sum)^((v0>>5)+k3);
		v0 -= ((v1<<4)+k0)^(v1+sum)^((v1>>5)+k1);
		sum -= delta;
	}
	*(uint32_t *)v = v0;
	*(uint32_t *)(v+4) = v1;
}

int main() {
	for(int i = 0; i<4; i++) decrypt(flag+i*8);
	printf("%s\n", flag);
	return 0;
}