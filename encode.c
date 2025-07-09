#include "encode.h"

struct data * encode(char *in_callsign, char *in_locator, uint8_t in_power) {
	struct data wspr_msg = {0};

	int callsign_offset = 0;
	int callsign_final_length = 0;
	int callsign_ch[6] = {0};
	
	/* Valid callsign characters */
	const char callsign_characters[] = 
		"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

	int locator_ch[4] = {0};	

	/*Valid locator characters*/
	const char locator_characters[] = "ABCDEFGHIJKLMNOPQR";
	const char locator_numbers[] = "0123456789";
		
	/* Third character in the call sign must be a number 
	   if not then add a space at the beginning          */
	if (!isdigit(in_callsign[2])) {
		wspr_msg.callsign[0] = ' ';
		callsign_offset++;
	}
	
	/* Check if the callsign is longer than max allowed length (6 chars) */
	if (strlen(in_callsign) >= MAX_CALLSIGN_LENGTH) {
		fprintf(stderr, "Callsign too long!\n");
		exit(EXIT_FAILURE);
	}

	/* Calculate the final callsign length top copy to the wspr message */
	callsign_final_length = MAX_CALLSIGN_LENGTH - callsign_offset;

	/* Copy the in callsign to the wspr message callsign */
	memcpy(&(wspr_msg).callsign[callsign_offset], in_callsign, callsign_final_length);

	/* If the callsign in less than max allowed then add padding spaces */
	for (int i=strlen(wspr_msg.callsign); i<MAX_CALLSIGN_LENGTH;i++)
		wspr_msg.callsign[i] = ' ';
	
	/*Find the number encoding for the characters in the callsign */
	for (int i=0;i<MAX_CALLSIGN_LENGTH;i++) {
		for (int p=0;p<strlen(callsign_characters); p++) {
			if (wspr_msg.callsign[i] == callsign_characters[p]) {
				callsign_ch[i] = p;
				break;
			} 
		}
	} 
	
	wspr_msg.n  = callsign_ch[0]; //N1
	wspr_msg.n = wspr_msg.n * 36 + callsign_ch[1]; //N2
	wspr_msg.n = wspr_msg.n * 10 + callsign_ch[2]; //N3
	wspr_msg.n = wspr_msg.n * 27 + callsign_ch[3] - 10; //N4
	wspr_msg.n = wspr_msg.n * 27 + callsign_ch[4] - 10; //N5
	wspr_msg.n = wspr_msg.n * 27 + callsign_ch[5] - 10; //N6
	
	memcpy(&(wspr_msg).locator, in_locator, MAX_LOCATOR_LENGTH);


	/*Find the number encoding for the characters in the locator */
	for (int i=0;i<MAX_LOCATOR_LENGTH;i++) {
		for (int p=0;p<strlen(locator_characters); p++) {
			if (wspr_msg.locator[i] == locator_characters[p]) {
				locator_ch[i] = p;
				break;
			}
		}
		for (int p=0;p<strlen(locator_numbers); p++) {
			if (wspr_msg.locator[i] == locator_numbers[p]) {
				locator_ch[i] = p;
				break;
			}
		}

	}
	
	/* Add locator to m */
	wspr_msg.m = (179 - 10 * locator_ch[0] - locator_ch[2]) * 180 + 10 * locator_ch[1] + locator_ch[3];
	
	/* Add power to m */
	wspr_msg.power = in_power;
	wspr_msg.m = wspr_msg.m * 128 + wspr_msg.power + 64;

	/* Bitpacking */
	wspr_msg.n = wspr_msg.n << 4;
	wspr_msg.bitpacked[0] = (wspr_msg.n & 0xFF000000) >> 24;
	wspr_msg.bitpacked[1] = (wspr_msg.n & 0xFF0000) >> 16;
	wspr_msg.bitpacked[2] = (wspr_msg.n & 0xFF00) >> 8;
	wspr_msg.bitpacked[3] = (wspr_msg.n & 0xFF);
	
	wspr_msg.m = wspr_msg.m << 2;
	wspr_msg.bitpacked[3] = wspr_msg.bitpacked[3] + ((wspr_msg.m & 0xF00000) >> 20);
	wspr_msg.bitpacked[4] = (wspr_msg.m & 0xFF000) >> 12;
	wspr_msg.bitpacked[5] = (wspr_msg.m & 0xFF0) >> 4;
	wspr_msg.bitpacked[6] = (wspr_msg.m & 0xF);
	wspr_msg.bitpacked[6] = wspr_msg.bitpacked[6] << 4;

	for (int i=0;i<11;i++)
		printf("%X ", wspr_msg.bitpacked[i]);
	printf("\n");

	return NULL;
}
