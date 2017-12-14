#include "ensitlm.h"
#include "generator.h"
#include "memory.h"
#include "bus.h"
#include "LCDC.h"
#include "ROM.h"

using namespace std;
using namespace sc_core;

int sc_main(int argc, char **argv) {
	
	/* Instanciation des modules */
	Generator Generator1("Generator1");
	Bus Bus("Bus");
	Memory Memory("Memory", MEMORY_SIZE_OCTET);
	LCDC LCDC("LCDC", sc_time(1.0 / 25, SC_SEC));
	ROM ROM("ROM");

	/* Mapping de la mémoire */
	Bus.map(Memory.target, ADDR_MEMORY_START, MEMORY_SIZE);
	Bus.map(LCDC.target_socket, ADDR_LCDC_START, LCDC_MEMORY_SIZE);
	Bus.map(ROM.socket, ADDR_ROM_START, ROM_MEMORY_SIZE_OCTET);
	

	/* Connection au bus */
	Generator1.Generator::initiator.bind(Bus.target);
	Bus.Bus::initiator.bind(Memory.target);
	Bus.Bus::initiator.bind(LCDC.target_socket);
	Bus.Bus::initiator.bind(ROM.socket);
	LCDC.initiator_socket.bind(Bus.target);

	/* Connection du signal d'interruption du LCDC au port du Generator */
	LCDC.display_int.bind(Generator1.irq_signal);

	/* Début de la simulation */
	sc_start();
	return 0;
}
