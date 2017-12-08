#include "generator.h"
#include "memory.h"
#include "LCDC_registermap.h"
#include "ROM.h"
#include <unistd.h>

using namespace std;

/**
 * Thread d'execution principal
 * Initialise l'écran et la mémoire
 * puis affiche l'image présent en ROM décalée d'un pixel vers le bas
 * retardée les 10 ms pour plus de visibilité
 */
void generator::thread(void) {
	unsigned int usec = 10000;

	/* Filling memory with ROM */
	store_image_in_memory(0x0);

	/* Initiating the Screen */
	ensitlm::data_t val = MEM_ADDR_START;
	ensitlm::addr_t addr = LCDC_ADDR_REG;
	initiator.write(addr, val);
	val = 0x1;
	addr = LCDC_START_REG;
	initiator.write(addr, val);

	int offset = ROW_SIZE;
	while (1) {
		/* Waiting for interrupt */
		wait(irq_interrupt);

		store_image_in_memory(offset);
		offset += ROW_SIZE;
		offset %= IMG_SIZE;

		usleep(usec);
		refresh();
	}
}

/**
 * Enregistre une image présente en ROM dans la mémoire avec un offset
 * Si l'image dépasse de la zone de mémoire,
 * les pixels sont enregistrés au début de la zone
 */
void generator::store_image_in_memory(int offset) {
	ensitlm::data_t val_rom;
	ensitlm::data_t val_mem1 = 0x0;
	ensitlm::data_t val_mem2 = 0x0;
	ensitlm::addr_t addr_rom = ROM_MAP_START;
	ensitlm::addr_t addr_mem = MEM_ADDR_START + offset;
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
		if (addr_mem >= MEM_ADDR_START + MEM_ADDR_SIZE) {
			addr_mem = MEM_ADDR_START;
		}
		initiator.write(addr_mem, val_mem1);
		addr_mem += 4;
		if (addr_mem >= MEM_ADDR_START + MEM_ADDR_SIZE) {
			addr_mem = MEM_ADDR_START;
		}
		initiator.write(addr_mem, val_mem2);
		addr_mem += 4;
		addr_rom += 4;
	}
}

/**
 * Notifie le handler sur changement du signal irq envoyé par le LCDC
 */
void generator::irq_handler(void) {
	irq_interrupt.notify();
}

/**
 * Ecrit dans le registre d'interrupt du LCDC
 * pour notifier qu'il peut refresh son buffer
 */
void generator::refresh(void) {
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
