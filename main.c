#include "helper.h"
#include <inttypes.h>

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
	pointer = (uint64_t*) ram;
	
	for (ID = 1; ID <= 3; ID++){
		tmp_pointer = (uint64_t*) tmp_buf;
		
		for (i = 0; i < 1024;){
			size = (int) *tmp_pointer >> 3 << 3;
			int curr_ID = (*tmp_pointer & 6) >> 1;
			
			if (size == 0)
				break;
			
			if (curr_ID == ID){
				memcpy(pointer, tmp_pointer, size); /* Copy the block into main memory */
				pointer += size / 8;
			}
			
			i += size;
			tmp_pointer += size / 8;
		}
	}
	
	/* All of the blocks should now be in order from lowest to highest ID, 
	 * we can now swap them based on their allocation
	 */
	 
	memmove(tmp_buf, ram, 1024); /* Move all blocks to temp memory */
	
	pointer = (uint64_t*) ram;
	tmp_pointer = (uint64_t*) tmp_buf;
	int tmp_size = 0;
	 
	for (ID = 1; ID <= 3;){
		int curr_ID = (int) (*tmp_pointer & 6) >> 1;
		size = 0;
		
		if (curr_ID == 0)
			break;
		
		if (curr_ID == ID){
			while(curr_ID == ID){
				size += ((int) *tmp_pointer >> 3 << 3);
				tmp_pointer += ((int) *tmp_pointer >> 3 << 3) / 8;
				curr_ID = (int) (*tmp_pointer & 6) >> 1;
			}
			
			tmp_pointer -= size / 8;
		
			for (i = 0; i < size;){
				tmp_size = (int) *tmp_pointer >> 3 << 3;
			
				if (((int) *tmp_pointer & 1) == 1){
					memcpy(pointer, tmp_pointer, tmp_size);
					pointer += tmp_size / 8;
				}
			
				i += tmp_size;
				tmp_pointer += tmp_size / 8;
			}
			
			tmp_pointer -= size / 8;
		
			for (i = 0; i < size;){
				tmp_size = ((int) *tmp_pointer >> 3 << 3);
			
				if (((int) *tmp_pointer & 1) == 0){
					memcpy(pointer, tmp_pointer, tmp_size);
					pointer += tmp_size / 8;
				}
			
				i += tmp_size;
				tmp_pointer += tmp_size / 8;
			}
			tmp_pointer = (uint64_t*) tmp_buf;
			ID++;
		}
		
		else{
			size += ((int) *tmp_pointer >> 3 << 3);
			tmp_pointer += ((int) *tmp_pointer >> 3 << 3) / 8;
			curr_ID = (int) (*tmp_pointer & 6) >> 1;
		}
	}
	
	/* All of the blocks should now be in order based on their ID, then their allocation flag, 
	 * we can now swap them based on their size
	 */
	
	memmove(tmp_buf, ram, 1024); /* Move all blocks to temp memory */
	
	pointer = (uint64_t*) ram;
	tmp_pointer = (uint64_t*) tmp_buf;
	tmp_size = 0;
	int flag = 0;
	int s = 0;
	
	for (ID = 1; ID <= 3;){
		int curr_ID = (int) (*tmp_pointer & 6) >> 1;
		size = 0;
		
		if (curr_ID == 0)
			break;
		
		if (curr_ID == ID){
			int flag_1 = 0;
			int size_1 = 0;
			int flag_0 = 0;
			
			while(curr_ID == ID){
				size += ((int) *tmp_pointer >> 3 << 3);
				if (((int) *pointer & 1) == 1){
					flag_1++;
					size_1 += ((int) *tmp_pointer >> 3 << 3);
				}
				else{
					flag_0++;
				}
				tmp_pointer += ((int) *tmp_pointer >> 3 << 3) / 8;
				curr_ID = (int) (*tmp_pointer & 6) >> 1;
			}
			
			tmp_pointer -= size / 8;
			
			tmp_pointer = (uint64_t*) tmp_buf;
			ID++;
		}
		
		else{
			size += ((int) *tmp_pointer >> 3 << 3);
			tmp_pointer += ((int) *tmp_pointer >> 3 << 3) / 8;
			curr_ID = (int) (*tmp_pointer & 6) >> 1;
		}
	}
	
	pointer = (uint64_t*) ram;
	ID = *pointer & 6 >> 1;

	while(ID != 0){
		int tmp_size = *pointer >> 3 << 3;
		pointer += tmp_size / 8;
		ID = *pointer & 6 >> 1;
	}
	uint64_t header = 16;
	*pointer = header;
	pointer++;
	*pointer = header;
	
    /*
     * Do not modify code below.
     */
    cse320_check();
    cse320_free();
    return ret;
}
