#include "helper.h"
#include <inttypes.h>

typedef struct header{
	uint64_t size;
	uint64_t ID;
	uint64_t Flag;
} Header;

int main(int argc, char** argv) {
    if (*(argv + 1) == NULL) {
        printf("You should provide name of the test file.\n");
        return 1;
    }
    void* ram = cse320_init(*(argv + 1));
    void* tmp_buf = cse320_tmp_buffer_init();
    int ret = 0;
    /*
     * You code goes below. Do not modify provided parts
     * of the code.
     */

	Header first_block_header;
	uint64_t* first_block =  (uint64_t*) ram;
	first_block_header.size = (*first_block >> 3) << 3;
	first_block_header.ID = (*first_block & 6) >> 1;
	first_block_header.Flag = *first_block & 1;

    /*
     * Do not modify code below.
     */
    cse320_check();
    cse320_free();
    return ret;
}
