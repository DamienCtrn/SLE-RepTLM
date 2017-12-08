#include "ensitlm.h"
#include "bus.h"
#include <systemc.h>

using namespace sc_core;

struct generator : sc_core::sc_module {
	ensitlm::initiator_socket<generator> initiator;
	sc_signal<bool, SC_MANY_WRITERS> irq_signal;
	sc_event irq_interrupt;
	void thread(void);
	void store_image_in_memory(ensitlm::addr_t addr_mem);
	void irq_handler(void);
	void refresh(void);
	SC_CTOR(generator);
};
