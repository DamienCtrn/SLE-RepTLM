/********************************************************************
 * Copyright (C) 2009, 2012 by Verimag                              *
 * Initial author: Matthieu Moy                                     *
 ********************************************************************/

#include "ensitlm.h"
#include "mb_wrapper.h"
#include "microblaze.h"
#include <iomanip>


/* Time between two step()s */
static const sc_core::sc_time PERIOD(20, sc_core::SC_NS);

// #define DEBUG
// #define INFO
#define STEPS_BETWEEN_IRQ 5

using namespace std;

MBWrapper::MBWrapper(sc_core::sc_module_name name)
    : sc_core::sc_module(name), irq("irq"),
      m_iss(0) /* identifier, not very useful since we have only one instance */
{
	m_iss.reset();
	m_iss.setIrq(false);
	SC_THREAD(run_iss);
	SC_METHOD(interrupt_handler);
	sensitive << irq;
}

void MBWrapper::interrupt_handler() {
	m_iss.setIrq(true);
}

void MBWrapper::exec_data_request(enum iss_t::DataAccessType mem_type,
                                  uint32_t mem_addr, uint32_t mem_wdata) {
	uint32_t localbuf;
	tlm::tlm_response_status status;
	switch (mem_type) {
	case iss_t::READ_WORD: {
#ifdef DEBUG
		std::cout << hex << "read word " << setw(10) << localbuf
		          << " at address " << mem_addr << std::endl;
#endif
		socket.read(mem_addr, localbuf);
		m_iss.setDataResponse(false, uint32_be_to_machine(localbuf));
	} break;
	case iss_t::READ_BYTE: {
		uint32_t modulus = mem_addr % 4;
		uint32_t addr_aligned = mem_addr - modulus;
		socket.read(addr_aligned, localbuf);
		localbuf = uint32_be_to_machine(localbuf);
		localbuf = ((localbuf) >> (modulus * 8)) & 0xFF;
		m_iss.setDataResponse(false, localbuf);
#ifdef DEBUG
		std::cout << hex << "read byte " << setw(10) << localbuf
				  << " at address " << addr_aligned << std::endl;
#endif
	} break;
	case iss_t::WRITE_HALF:
	case iss_t::WRITE_BYTE:
	case iss_t::READ_HALF:
		// Not needed for our platform.
		std::cerr << "Operation " << mem_type << " unsupported for "
		          << std::showbase << std::hex << mem_addr << std::endl;
		abort();
	case iss_t::LINE_INVAL:
		// No cache => nothing to do.
		break;
	case iss_t::WRITE_WORD: {
#ifdef DEBUG
		std::cout << hex << "wrote   " << setw(10) << mem_wdata
		          << " at address " << mem_addr << std::endl;
#endif
		socket.write(mem_addr, uint32_be_to_machine(mem_wdata));
		m_iss.setDataResponse(0, 0);
	} break;
	case iss_t::STORE_COND:
		break;
	case iss_t::READ_LINKED:
		break;
	}
}

void MBWrapper::run_iss(void) {

	int inst_count = 0;

	while (true) {
		if (m_iss.isBusy())
			m_iss.nullStep();
		else {
			bool ins_asked;
			uint32_t ins_addr;
			m_iss.getInstructionRequest(ins_asked, ins_addr);

			if (ins_asked) {
				uint32_t localbuf;
				socket.read(ins_addr, localbuf);
				m_iss.setInstruction(false, uint32_be_to_machine(localbuf));
#ifdef DEBUG
		std::cout << hex << "read    " << setw(10) << localbuf
		          << " at address " << ins_addr << std::endl;
#endif

			}

			bool mem_asked;
			enum iss_t::DataAccessType mem_type;
			uint32_t mem_addr;
			uint32_t mem_wdata;
			m_iss.getDataRequest(mem_asked, mem_type, mem_addr,
			                     mem_wdata);

			if (mem_asked) {
				exec_data_request(mem_type, mem_addr,
				                  mem_wdata);
			}
			m_iss.step();
			// Aquittement de l'interruption
			inst_count++;
			if (inst_count > STEPS_BETWEEN_IRQ) {
				inst_count = 0;
				m_iss.setIrq(false);
			}
		}

		wait(PERIOD);
	}
}
