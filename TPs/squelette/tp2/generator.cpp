#include "generator.h"

using namespace std;

void generator::thread(void) {

}

generator::generator(sc_core::sc_module_name name) : sc_core::sc_module(name) {
	SC_THREAD(thread);
}
