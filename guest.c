#include <stddef.h>
#include <stdint.h>

#define INT_MAX 2147483647

static inline void outb(uint16_t port, uint32_t value) {
    asm("out %0,%1" : /* empty */ : "a" (value), "Nd" (port) : "memory");
}

static inline uint32_t inb(uint16_t port) {
    uint32_t ret;
    asm("in %1, %0" : "=a"(ret) : "Nd"(port) : "memory" );
    return ret;
}

void _outb_virtualaddr(uint16_t port, void* addr) {
	uintptr_t virtual_addr = (uintptr_t)(addr);
	if (INT_MAX < virtual_addr) {
		return;
	}
	outb(port, virtual_addr);
}

void print(const char *str) {
	_outb_virtualaddr(0xAA, (void*)(str));
}

int exits(void) {
	uint32_t ret = inb(0xBB);
	if (INT_MAX < ret) {
		return -1;
	}
	return (int)(ret);
}

void open(const char *str) {
	_outb_virtualaddr(0xCC, (void*)(str));
}

int read(void *buf, int len) {
	uintptr_t virtual_addr1 = (uintptr_t)(void*)(buf);
	uintptr_t virtual_addr2 = (uintptr_t)(void*)(&len);
	if (INT_MAX < virtual_addr1 || INT_MAX < virtual_addr2) {
		return -1;
	}
	uintptr_t args[] = {virtual_addr1, virtual_addr2};
	outb(0xDD, (uintptr_t) args);
	// len contain updated ret
	return len;
}

int write(void *buf, int len) {
	uintptr_t virtual_addr1 = (uintptr_t)(void*)(buf);
	uintptr_t virtual_addr2 = (uintptr_t)(void*)(&len);
	if (INT_MAX < virtual_addr1 || INT_MAX < virtual_addr2) {
		return -1;
	}
	uintptr_t args[] = {virtual_addr1, virtual_addr2};
	outb(0xEE, (uintptr_t) args);
	// len contain updated ret
	return len;
}

void close() {
	outb(0xFF, -1);
}

// bonus
void seek_start() {
	outb(0x00, -1);
}

void
__attribute__((noreturn))
__attribute__((section(".start")))
_start(void) {
	int before = exits();
	print("Hello, world!\n");
	int after = exits();
	
	int diff = after-before;
	char *print_buff = "Exits diff is: -\n";
	print_buff[15] = '0'+diff;
	print(print_buff);

	open("/tmp/new.txt");
	char *read_buff = "00000000000000000000";
	char *write_buff = "11111111111111111111";
	int ret = write(write_buff, 20);
	if (20 == ret) {
		print("got write ret of 20, wrote: ");
		print(write_buff);
		print("\n");
	}

	seek_start();
	ret = read(read_buff, 20);
	if (20 == ret) {
		print("got read ret of 20, read buff is: ");
		print(read_buff);
		print("\n");
	}
	close();

	*(long *) 0x400 = 42;
	for (;;)
		asm("hlt" : /* empty */ : "a" (42) : "memory");
}
