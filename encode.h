#ifndef ENCODE_H
#define ENCODE_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_CALLSIGN_LENGTH 6
#define MAX_LOCATOR_LENGTH 4

struct data {
	char callsign[MAX_CALLSIGN_LENGTH];
	char locator[MAX_LOCATOR_LENGTH];
	uint8_t power;
	uint32_t n;
	uint32_t m;
	uint8_t bitpacked[11];
	uint8_t convolution_encoded[21];
	uint8_t interleaving[21];
	uint8_t merged_vector[21];

};


struct data * encode(char *in_callsign, char *in_locator, uint8_t in_power);

#endif
