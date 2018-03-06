#include "helper.h"
#include <inttypes.h>

void swap(uint64_t* pointer, uint64_t* tmp_pointer, int size){
	memmove(tmp_pointer, pointer, size);
	int tmp_size = (int) *(pointer + size / 8) >> 3 << 3;
	memmove(pointer, pointer + size / 8, tmp_size);
	memmove(pointer + tmp_size / 8, tmp_pointer, size);
}

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

	int i;
	int buf_size = 128;
	int total_size = 0;
	uint64_t* pointer = (uint64_t*) ram;
	uint64_t* tmp_pointer = (uint64_t*) tmp_buf;
	cse320_sbrk(896);
	
	/* If pointer is at the beginning of the header:
	 * size = *pointer >> 3 << 3
	 * ID = *pointer & 6 >> 1
	 * flag = *pointer & 1
	 */
	 
	memmove(tmp_buf, ram, 1024); /* Move all blocks to temp memory */
	
	/* Now we can just iterate through each memory block, checking the ID, 
	 * and over-writing main memory if we find one of the right ID
	 */
	
	int ID;
	int size = 0;
	int flag = 0;
	pointer = (uint64_t*) ram;
	
	for (ID = 1; ID <= 3; ID++){
		tmp_pointer = (uint64_t*) tmp_buf;
		
		for (i = 0; i < 1024;){
			size = (int) *tmp_pointer >> 3 << 3;
			int curr_ID = (*tmp_pointer & 6) >> 1;
			
			if (size == 0)
				break;
			
			if (curr_ID == ID){
				memmove(pointer, tmp_pointer, size); /* Copy the block into main memory */
				pointer += size / 8;
			}
			
			i += size;
			tmp_pointer += size / 8;
		}
	}
	
	/* All of the blocks should now be in order from lowest to highest ID, 
	 * we can now swap them based on their allocation
	 */
	 
	tmp_pointer = (uint64_t*) tmp_buf;
	i = 0;
	 
	for (ID = 1; ID <= 3; ID++){
		pointer = (uint64_t*) ram;
		int curr_ID = (*pointer & 6) >> 1;
		
		while(curr_ID == ID){
			size = (int) *pointer >> 3 << 3;
			flag = *pointer & 1;
			
			// CHANGE THIS SO IT KEEPS MOVING IT BACK UNTIL IT'S AT THE 1 BLOCK
			if (flag == 0){
				int tmp_flag = (*(pointer + size / 8) & 6) >> 1;
				int tmp_ID = (*(pointer + size / 8) & 6) >> 1;
				if (tmp_flag == 1 && tmp_ID == curr_ID){ /* We have to swap them */
					swap(pointer, tmp_pointer, size);
				}
				else
					pointer += size / 8;
			}
			else{
				pointer += size / 8;
			}
			
			i += size;
			curr_ID = (*pointer & 6) >> 1;
		}
	}
	
	/* All of the blocks should now be in order based on their ID, then their allocation flag, 
	 * we can now swap them based on their size
	 */
	 
	for (ID = 1; ID <= 3; ID++){
		pointer = (uint64_t*) ram;
		int curr_ID = (*pointer & 6) >> 1;
		int flag = 1;
		
		for (flag = 1; flag >= 0; flag--){
			
		}
	}
	
    /*
     * Do not modify code below.
     */
    cse320_check();
    cse320_free();
    return ret;
}
