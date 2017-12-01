#include "generator.h"
#include "memory.h"
#include "LCDC_registermap.h"
#include "ROM.h"

using namespace std;

void generator::thread(void) {

	/* Filling memory with ROM */
	ensitlm::data_t val_rom;
	ensitlm::data_t val_mem1 = 0x0;
	ensitlm::data_t val_mem2 = 0x0;
	ensitlm::addr_t addr_rom = ROM_MAP_START;
	ensitlm::addr_t addr_mem = MEM_ADDR_START;
	while (addr_rom < ROM_MAP_START + ROM_SIZE) {
		initiator.read(addr_rom, val_rom);
		val_mem1 =  (val_rom & 0xF0000000)
				 + ((val_rom & 0x0F000000) >> 4)
				 + ((val_rom & 0x00F00000) >> 8)
				 + ((val_rom & 0x000F0000) >> 12);
		val_mem2 = ((val_rom & 0x0000F000) << 16)
				 + ((val_rom & 0x00000F00) << 12)
				 + ((val_rom & 0x000000F0) << 8)
				 + ((val_rom & 0x0000000F) << 4);
		initiator.write(addr_mem, val_mem1);
		addr_mem += 4;
		initiator.write(addr_mem, val_mem2);
		addr_mem += 4;
		addr_rom += 4;
	}

	/* Initiating the Screen */
	ensitlm::data_t val = MEM_ADDR_START;
	ensitlm::addr_t addr = LCDC_ADDR_REG;
	initiator.write(addr, val);
	val = 0x1;
	addr = LCDC_START_REG;
	initiator.write(addr, val);
}

generator::generator(sc_core::sc_module_name name) : sc_core::sc_module(name) {
	SC_THREAD(thread);
	irq_signal = sc_signal<bool, SC_MANY_WRITERS>("IRQ");
}
