#ifndef ENCODE_H
#define ENCODE_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_CALLSIGN_LENGTH 6
#define MAX_LOCATOR_LENGTH 4
#define WSPR_BIT_LENGTH 162
#define WSPR_UNCODED_MSG_LENGTH 11
#define BITS_IN_BYTE 8
#define BITS_IN_DWORD 32
#define MAX_BYTE_NUMERIC_VALUE 255

struct data {
	char callsign[MAX_CALLSIGN_LENGTH];
	char locator[MAX_LOCATOR_LENGTH];
	uint8_t power;
	uint32_t n;
	uint32_t m;
	uint8_t bitpacked[WSPR_UNCODED_MSG_LENGTH];
	uint8_t convolution_encoded[WSPR_BIT_LENGTH];
	uint8_t interleaving[WSPR_BIT_LENGTH];
	uint8_t merged_vector[WSPR_BIT_LENGTH];

};


struct data * encode(char *in_callsign, char *in_locator, uint8_t in_power);

#endif
