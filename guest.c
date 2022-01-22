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

void print(const char *str) {
	uintptr_t virtual_addr = (uintptr_t)(void*)(str);
	if (INT_MAX < virtual_addr) {
		return;
	}
	outb(0xAA, virtual_addr);
}

int exits(void) {
	uint32_t ret = inb(0xBB);
	if (INT_MAX < ret) {
		return -1;
	}
	return (int)(ret);
}

void
__attribute__((noreturn))
__attribute__((section(".start")))
_start(void) {
	char *s = "Hello, world!\n";
	char *buff = "Exits diff is: -\n";
	int before = exits();
	print(s);
	int after = exits();
	int diff = after-before;
	buff[15] = '0'+diff-1; // minus exits's exit
	print(buff);

	*(long *) 0x400 = 42;
	for (;;)
		asm("hlt" : /* empty */ : "a" (42) : "memory");
}
