#include "generator.h"

using namespace std;

void generator::thread(void) {
	ensitlm::data_t val_w = 1;
	ensitlm::data_t val_r = 0;
	ensitlm::addr_t addr = 0x10000000;
	// Test Auto
	for (size_t i = 0; i < 0x40; i++) {
		socket.write(addr, val_w);
		socket.read(addr, val_r);
		if (val_r != val_w) {
			std::cout << "ERREUR MEM @" << addr << '\n';
		}
		val_w ++;
		addr += 4;
	}
}

generator::generator(sc_core::sc_module_name name) : sc_core::sc_module(name) {
	SC_THREAD(thread);
}
