#include "encode.h"

static void print_array(uint8_t *array, int length);

struct data * encode(char *in_callsign, char *in_locator, uint8_t in_power) {
	struct data *wspr_msg = (struct data *) calloc(1, sizeof(struct data));

	int callsign_offset = 0;
	int callsign_final_length = 0;
	int callsign_ch[6] = {0};

	uint32_t reg0 = 0;
	uint32_t reg1 = 0;
	uint8_t next_bit = 0;
	uint8_t single_parity_bit = 0;
	uint32_t and_result = 0;
	uint8_t bit_length = 0;

	uint8_t P = 0;
	uint8_t J = 0;	
	uint8_t I = 0;

	const uint8_t sync_vector[] = {
					1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0,
	              			1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0,
	              			0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 1,
	             			0, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0,
	              			1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1,
	              			0, 0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 0, 1,
	              			1, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0,
	          			1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0					
					};

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
		wspr_msg->callsign[0] = ' ';
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
	memcpy(&(wspr_msg->callsign)[callsign_offset], in_callsign, callsign_final_length);

	/* If the callsign in less than max allowed then add padding spaces */
	for (int i=strlen(wspr_msg->callsign); i<MAX_CALLSIGN_LENGTH;i++)
		wspr_msg->callsign[i] = ' ';
	
	/*Find the number encoding for the characters in the callsign */
	for (int i=0;i<MAX_CALLSIGN_LENGTH;i++) {
		for (int p=0;p<strlen(callsign_characters); p++) {
			if (wspr_msg->callsign[i] == callsign_characters[p]) {
				callsign_ch[i] = p;
				break;
			} 
		}
	} 
	
	wspr_msg->n  = callsign_ch[0]; //N1
	wspr_msg->n = wspr_msg->n * 36 + callsign_ch[1]; //N2
	wspr_msg->n = wspr_msg->n * 10 + callsign_ch[2]; //N3
	wspr_msg->n = wspr_msg->n * 27 + callsign_ch[3] - 10; //N4
	wspr_msg->n = wspr_msg->n * 27 + callsign_ch[4] - 10; //N5
	wspr_msg->n = wspr_msg->n * 27 + callsign_ch[5] - 10; //N6
	
	memcpy(&(wspr_msg->locator), in_locator, MAX_LOCATOR_LENGTH);


	/*Find the number encoding for the characters in the locator */
	for (int i=0;i<MAX_LOCATOR_LENGTH;i++) {
		for (int p=0;p<strlen(locator_characters); p++) {
			if (wspr_msg->locator[i] == locator_characters[p]) {
				locator_ch[i] = p;
				break;
			}
		}
		for (int p=0;p<strlen(locator_numbers); p++) {
			if (wspr_msg->locator[i] == locator_numbers[p]) {
				locator_ch[i] = p;
				break;
			}
		}

	}
	
	/* Add locator to m */
	wspr_msg->m = (179 - 10 * locator_ch[0] - locator_ch[2]) * 180 + 10 * locator_ch[1] + locator_ch[3];
	
	/* Add power to m */
	wspr_msg->power = in_power;
	wspr_msg->m = wspr_msg->m * 128 + wspr_msg->power + 64;

	/* Bitpacking */
	wspr_msg->n = wspr_msg->n << 4;
	wspr_msg->bitpacked[0] = (wspr_msg->n & 0xFF000000) >> 24;
	wspr_msg->bitpacked[1] = (wspr_msg->n & 0xFF0000) >> 16;
	wspr_msg->bitpacked[2] = (wspr_msg->n & 0xFF00) >> 8;
	wspr_msg->bitpacked[3] = (wspr_msg->n & 0xFF);
	
	wspr_msg->m = wspr_msg->m << 2;
	wspr_msg->bitpacked[3] = wspr_msg->bitpacked[3] + ((wspr_msg->m & 0xF00000) >> 20);
	wspr_msg->bitpacked[4] = (wspr_msg->m & 0xFF000) >> 12;
	wspr_msg->bitpacked[5] = (wspr_msg->m & 0xFF0) >> 4;
	wspr_msg->bitpacked[6] = (wspr_msg->m & 0xF);
	wspr_msg->bitpacked[6] = wspr_msg->bitpacked[6] << 4;

	/* Extract MSB from the bitpacked data stream to be used in convolutional encoding */
	for (int i=0; i<WSPR_UNCODED_MSG_LENGTH;i++) {
		for (int x=0;x<BITS_IN_BYTE;x++) {
			next_bit = (((wspr_msg->bitpacked[i] << x) & 0x80) == 0x80) ? 1 : 0;
			reg0 = reg0 << 1;
			reg1 = reg1 << 1;
			reg0 |= next_bit;
			reg1 |= next_bit;
			
	//		and_result = 0;
			and_result = reg0 & 0xF2D05351;
			single_parity_bit = 0;
			
			for (int bit=0; bit<BITS_IN_DWORD;bit++) {
				single_parity_bit = (uint8_t) single_parity_bit ^ (and_result & 0x01);
				and_result = and_result >> 1;
			}
			wspr_msg->convolution_encoded[bit_length] = single_parity_bit;
			bit_length++;
			
			and_result = 0;
			and_result = reg1 & 0xE4613C47;
			single_parity_bit = 0;

			for (int bit=0;bit<BITS_IN_DWORD;bit++) {
				single_parity_bit = (uint8_t) single_parity_bit ^ (and_result & 0x01);
				and_result = and_result >> 1;
			}
			wspr_msg->convolution_encoded[bit_length] = single_parity_bit;
			bit_length++;

			if (bit_length >= WSPR_BIT_LENGTH)
				break;

 		}
	}

	/* Interleaving process */
	for (int i=0; i<MAX_BYTE_NUMERIC_VALUE;i++) {
		
		I = i;
		J = 0;
		for (int bit=0; bit<BITS_IN_BYTE; bit++) {
			if ((I & 0x01) > 0)
				J = J | (1 << (7 - bit));
			I >>= 1;	
		}
			
		if (J < WSPR_BIT_LENGTH) {
			wspr_msg->interleaving[J] = wspr_msg->convolution_encoded[P];
			P++;
		} 

		if (P >= WSPR_BIT_LENGTH) 
			break;
	}

	/* Merge with sync vector */
	for (int i=0;i<WSPR_BIT_LENGTH;i++) {
		wspr_msg->merged_vector[i] =  wspr_msg->interleaving[i] * 2 + sync_vector[i];
	}	
			
	return wspr_msg;
}



void print(struct data *msg) {
	
	printf("Callsign: ");
	for (int i=0;i<MAX_CALLSIGN_LENGTH;i++) {
		printf("%c", msg->callsign[i]);
	}
	printf("\n");

	printf("Locator: ");
	for (int i=0;i<MAX_LOCATOR_LENGTH;i++) {
		printf("%c", msg->locator[i]);
	}
	printf("\n");

	printf("Power: %d\n\n", msg->power);
	
	printf("Bitpacked data: ");
	for (int i=0;i<WSPR_UNCODED_MSG_LENGTH;i++) {
		printf("0x%X ", msg->bitpacked[i]);
	}
	printf("\n\n");

	printf("Convolution encoded data: \n");	
	print_array(msg->convolution_encoded, WSPR_BIT_LENGTH);	
	
	printf("\n\n");
	printf("Interleaved data: \n");	
	print_array(msg->interleaving, WSPR_BIT_LENGTH);

	printf("\n\n");
	printf("Vector merged data: \n");	
	print_array(msg->merged_vector, WSPR_BIT_LENGTH);	
}



static void print_array(uint8_t *array, int length) {
	int line_items = 0;
	for (int i=0;i<length;i++) {
		if (line_items == 10) {
			printf("\n");
			line_items = 0;
		}

		printf("0x%X ", array[i]);
		line_items++;
	}
	printf("\n");
}
