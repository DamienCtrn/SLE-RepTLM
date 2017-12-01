#include "ensitlm.h"
#include "bus.h"

struct generator : sc_core::sc_module {
	ensitlm::initiator_socket<generator> initiator;
	void thread(void);

	SC_CTOR(generator);
};
