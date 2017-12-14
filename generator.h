#ifndef GENERATOR_H
#define GENERATOR_H

#include "ensitlm.h"
#include "bus.h"
#include "LCDC.h"

// Longueur de l'ecran en pixels
#define SCREEN_LENGTH 320
// Largeur de l'écran en pixels
#define SCREEN_WIDTH 240

// Taille mémoire en octets pour le composant Memory (partie video seulement)
#define VIDEO_MEMORY_SIZE_OCTET 76800
// Taille mémoire en octets pour le composant Memory (entier)
#define MEMORY_SIZE_OCTET 87040

// Taille de la plage mémoire destinée au bus pour le composant Memory
#define MEMORY_SIZE 0x15400
// Adresse de début d'allocation pour le composant Memory
#define ADDR_MEMORY_START 0x10000000
// Taille de la plage mémoire (10Ko) réservée au logiciel embarqué
#define SOFT_MEMORY_SIZE 0x2800

// Taille mémoire à réserver pour le LCDC (3 registres)
#define LCDC_MEMORY_SIZE 0xC
// Adresse de début d'allocation pour le composant LCDC
#define ADDR_LCDC_START ADDR_MEMORY_START + MEMORY_SIZE

// Taille mémoire à réserver pour la ROM
#define ROM_MEMORY_SIZE_OCTET 38400
// Adresse de début d'allocation pour le composant ROM
#define ADDR_ROM_START 0x0

using namespace sc_core;

SC_MODULE(Generator) {
	ensitlm::initiator_socket<Generator> initiator;
	sc_signal<bool, SC_MANY_WRITERS> irq_signal;
	sc_core::sc_event state_change_int;
	
	void thread(void);
	void load_picture(void);
	void shift_picture(void);
	void shift_line(int num_line);
	void notify_unlock(void);
	
	SC_CTOR(Generator) {
		SC_THREAD(thread);
		SC_METHOD(notify_unlock);
		sensitive << irq_signal;
	}
};

#endif
