#include <stdio.h>
#include <stdlib.h>
#include "encode.h"

int main(void) {

	struct data *wspr_msg = NULL;
	wspr_msg = encode("AJ4VD", "EL89", 30);
	print(wspr_msg);
	free(wspr_msg);

	return EXIT_SUCCESS;
}
