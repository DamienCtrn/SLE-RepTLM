#include "ensitlm.h"
#include "bus.h"

struct generator : sc_core::sc_module {
	ensitlm::initiator_socket<generator> socket;
	void thread(void);

	SC_CTOR(generator);
};
