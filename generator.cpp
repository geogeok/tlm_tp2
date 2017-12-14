#include "generator.h"
#include "LCDC.h"
#include "LCDC_registermap.h"

using namespace std;
using namespace ensitlm;

void Generator::thread(void) {

	// Ecriture dans le registre d'adresse
	initiator.write(ADDR_LCDC_START + LCDC_ADDR_REG,
			ADDR_MEMORY_START+SOFT_MEMORY_SIZE);
	// Lancement du LCDC
        initiator.write(ADDR_LCDC_START + LCDC_START_REG,0x00000001);
	// Ecriture dans le registre d'interruption
	initiator.write(ADDR_LCDC_START + LCDC_INT_REG,0x00000000);

	load_picture();
	cout << "Fin chargement de l'image" << endl;
	
	while(true) {
		// Attente de l'interruption
		wait(state_change_int);

		shift_picture();

		// Remise à 0 du signal du registre d'interruption
		initiator.write(ADDR_LCDC_START + LCDC_INT_REG,0x00000000);
	}
	
}

void Generator::load_picture() {
	// Variable qui récupèrera le contenu de la ROM
	data_t data;
	unsigned int mot1 = 0;
	unsigned int mot2 = 0;
	unsigned int masque1 = 0xF0000000;
	unsigned int masque2 = 0x0000F000;
	
	// Ecriture dans la mémoire vidéo pour stocker l'image
	for(int i = 0 ; i < ROM_MEMORY_SIZE_OCTET; i+=4) {
		initiator.read(ADDR_ROM_START+i,data);
		// Initialisation des variables
		mot1 = 0;
		mot2 = 0;
		masque1 = 0xF0000000;
		masque2 = 0x0000F000;
		for(int j = 0 ; j < 4 ; j++) {
			mot1 = ((data & masque1) >> (j*4)) + mot1;
			mot2 = ((data & masque2) << ((4-j)*4)) + mot2;
			masque1 = masque1 >> 4;
			masque2 = masque2 >> 4;
		}
		initiator.write(ADDR_MEMORY_START+SOFT_MEMORY_SIZE+(2*i),
				mot1);
		initiator.write(ADDR_MEMORY_START+SOFT_MEMORY_SIZE+(2*i+4),
				mot2);
	}
}

void Generator::shift_picture() {
	// Variable qui récupèrera le contenu de la mémoire vidéo
	data_t data;
	// Tableau qui contiendra la dernière ligne de l'ecran a copier en haut
	data_t buffer[SCREEN_LENGTH/4];

	// i < SCREEN_LENGTH/4 car on a 1 octet par pixel et 1 mot fait 4 octets
	for(int i = 0 ; i < SCREEN_LENGTH/4 ; i++) {
		initiator.read(ADDR_MEMORY_START + SOFT_MEMORY_SIZE +
			       (SCREEN_WIDTH-1)*SCREEN_LENGTH+(i*4),data);
		buffer[i] = data;
	}

	// On décale chacune des lignes sauf la dernière
	for(int j = SCREEN_WIDTH-2 ; j >= 0 ; j--) {
		shift_line(j);
	}

	// On recopie la ligne stockée en haut de l'écran
	for(int i = 0 ; i < SCREEN_LENGTH/4 ; i++) {
		initiator.write(ADDR_MEMORY_START + SOFT_MEMORY_SIZE + i*4,
				buffer[i]);
	}
}

void Generator::shift_line(int num_line) {
	data_t data;

	// On écrit la ligne num_line à la ligne suivante sur l'écran
	for(int i = 0 ; i < SCREEN_LENGTH ; i+=4) {
		initiator.read(ADDR_MEMORY_START + SOFT_MEMORY_SIZE +
			       num_line*SCREEN_LENGTH + i,data);
		initiator.write(ADDR_MEMORY_START + SOFT_MEMORY_SIZE +
				(num_line+1)*SCREEN_LENGTH + i,data);
	}
}

void Generator::notify_unlock() {
	if (irq_signal == true) {
		cout << "Unlocking the main thread" << endl;
		state_change_int.notify();
	}
}
