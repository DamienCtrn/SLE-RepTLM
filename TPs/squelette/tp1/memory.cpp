#include "memory.h"

using namespace std;

tlm::tlm_response_status memory::write(const ensitlm::addr_t &a,
                                       const ensitlm::data_t &d) {
	if ((a % 4) == 0 && a < size) { // accès alligné et dans la plage
		cout << name() << " - Writting : " << d << " At : " << a << endl;
		storage[a / 4] = d;	// stockage
		return tlm::TLM_OK_RESPONSE;
	} else {
		SC_REPORT_ERROR("TLM", "Accès non Autorisé");
		abort();
	}
}

tlm::tlm_response_status memory::read(const ensitlm::addr_t &a,
                                      /* */ ensitlm::data_t &d) {
	if ((a % 4) == 0 && a < size) { // accès alligné et dans la plage
  		cout << name() << " - Reading At : " << a << endl;
		d = storage[a / 4];	// lecture
  		return tlm::TLM_OK_RESPONSE;
  	} else {
  		SC_REPORT_ERROR("TLM", "Accès non Autorisé");
  		abort();
  	}
}

memory::memory(sc_core::sc_module_name name, int mem_size):
				sc_core::sc_module(name),
				size(mem_size){
	storage = new ensitlm::data_t[mem_size / 4];
}

memory::~memory(){
	delete storage;
}
