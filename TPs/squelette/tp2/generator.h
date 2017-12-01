#include "ensitlm.h"
#include "bus.h"
#include <systemc.h>

using namespace sc_core;

struct generator : sc_core::sc_module {
	ensitlm::initiator_socket<generator> initiator;
	sc_signal<bool, SC_MANY_WRITERS> irq_signal;
	void thread(void);

	SC_CTOR(generator);
};
