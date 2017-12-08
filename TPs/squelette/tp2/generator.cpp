#include "generator.h"
#include "memory.h"
#include "LCDC_registermap.h"
#include "ROM.h"

using namespace std;

void generator::thread(void) {

	/* Filling memory with ROM */
	store_image_in_memory(MEM_ADDR_START);

	/* Initiating the Screen */
	ensitlm::data_t val = MEM_ADDR_START;
	ensitlm::addr_t addr = LCDC_ADDR_REG;
	initiator.write(addr, val);
	val = 0x1;
	addr = LCDC_START_REG;
	initiator.write(addr, val);

	ensitlm::addr_t i = 0xF0;
	while (1) {
		/* Waiting for interrupt */
		wait(irq_interrupt);
		store_image_in_memory(MEM_ADDR_START + i);
		i += 0xF0;
		i %= (MEM_ADDR_START + MEM_ADDR_SIZE);
		refresh();
	}
}

void generator::store_image_in_memory(ensitlm::addr_t addr_mem) {
	ensitlm::data_t val_rom;
	ensitlm::data_t val_mem1 = 0x0;
	ensitlm::data_t val_mem2 = 0x0;
	ensitlm::addr_t addr_rom = ROM_MAP_START;
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
		if (addr_mem >= MEM_ADDR_START + MEM_ADDR_SIZE) {
			addr_mem -= MEM_ADDR_SIZE;
		}
		initiator.write(addr_mem, val_mem2);
		addr_mem += 4;
		if (addr_mem >= MEM_ADDR_START + MEM_ADDR_SIZE) {
			addr_mem -= MEM_ADDR_SIZE;
		}
		addr_rom += 4;
	}
}

void generator::irq_handler(void){
	irq_interrupt.notify();
}

void generator::refresh(void){
	ensitlm::addr_t addr = LCDC_INT_REG;
	initiator.write(addr, 0x0);
}

generator::generator(sc_core::sc_module_name name) : sc_core::sc_module(name) {
	SC_THREAD(thread);
	SC_METHOD(irq_handler);
	sensitive << irq_signal;
	irq_signal = sc_signal<bool, SC_MANY_WRITERS>("IRQ");
	sc_event irq_interrupt;
}
