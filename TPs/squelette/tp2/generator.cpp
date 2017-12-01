#include "generator.h"
#include "memory.h"
#include "LCDC_registermap.h"

using namespace std;

void generator::thread(void) {
	ensitlm::data_t val;
	ensitlm::addr_t addr;

	/* Filling memory */
	val = 0x004488CC;
	addr = MEM_ADDR_START;
	while (addr < MEM_ADDR_START + IMG_SIZE) {
		initiator.write(addr, val);
		addr += 4;
	}

	/* Initiating the Screen */
	val = MEM_ADDR_START;
	addr = LCDC_ADDR_REG;
	initiator.write(addr, val);
	val = 0x1;
	addr = LCDC_START_REG;
	initiator.write(addr, val);
}

generator::generator(sc_core::sc_module_name name) : sc_core::sc_module(name) {
	SC_THREAD(thread);
	irq_signal = sc_signal<bool, SC_MANY_WRITERS>("IRQ");
}
