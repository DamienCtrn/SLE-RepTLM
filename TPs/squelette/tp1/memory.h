#include "ensitlm.h"
#include "bus.h"

struct memory : sc_core::sc_module {

	unsigned int size; // en octets
	ensitlm::data_t * storage; // = unsigned int = 4 octets

	ensitlm::target_socket<memory> socket;
	tlm::tlm_response_status write(const ensitlm::addr_t &a,
	                               const ensitlm::data_t &d);

	tlm::tlm_response_status read(const ensitlm::addr_t &a,
	                              /* */ ensitlm::data_t &d);

	memory(sc_core::sc_module_name name, int mem_size);
	~memory();
};
