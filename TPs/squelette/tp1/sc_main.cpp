#include "ensitlm.h"
#include "generator.h"
#include "memory.h"
#include "bus.h"

int sc_main(int argc, char **argv) {
	(void)argc;
	(void)argv;
	/*
		GENERATOR -> BUS -> MEMORY
	*/

	generator g("Gérard");
	Bus b("Bertrand");
	memory m("Marco", 0xFC);

	/* Marco is mapped at addresses [0x10000000, 0x100000FF[, i.e. ... */
	b.map(m.socket, 0x10000000, 0xFF); // (socket, addresse de début, taille)

	/* connect components to the bus */
	g.socket.bind(b.target);
	b.initiator.bind(m.socket);

	/* and start simulation */
	sc_core::sc_start();
	return 0;
}
