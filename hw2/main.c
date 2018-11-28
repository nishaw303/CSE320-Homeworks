#include "helper.h"
#include <inttypes.h>

void sort(uint64_t* tmp_buf, uint64_t* block_start, int tot_size);
void swap_adj(uint64_t* tmp_buf, uint64_t* first_block, uint64_t* second_block);

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
	
	int num_of_blocks = 0;
	int i;
	uint64_t* pointer = (uint64_t*) ram;
	uint64_t* tmp_pointer = (uint64_t*) tmp_buf;
	cse320_sbrk(896);
	
	/* If pointer is at the beginning of the header:
	 * size = *pointer >> 3 << 3
	 * ID = *pointer & 6 >> 1
	 * flag = *pointer & 1
	 */
	 
	memmove(tmp_buf, ram, 1024); /* Move all blocks to temp memory */
	
	/* First we should remove any padding that would make things more difficult later */
	
	int itr;
	for (itr = 0; itr < 1024;){
		int tmp_size = (int) *tmp_pointer >> 3 << 3;
		int tmp_ID = (int) (*tmp_pointer & 6) >> 1;
		int tmp_flag = (int) *tmp_pointer & 1;
		if ((tmp_size < 1024) && (tmp_ID > 0 && tmp_ID < 4) && (tmp_flag >= 0 && tmp_flag <= 1)){
			memmove(pointer, tmp_pointer, tmp_size);
			num_of_blocks++;
			tmp_pointer += tmp_size / 8;
			pointer += tmp_size / 8;
			itr += tmp_size;
		}
		else{
			tmp_pointer++;
			itr += 8;
		}
	}
	
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
	
	
	/* And now we can coalesce any non-allocated blocks with the same ID together */
	
	pointer = (uint64_t*) ram;
	
	for (i = 0; i < num_of_blocks; i++){
		int tmp_flag = (int) *pointer & 1;
		int tmp_ID = (int) (*pointer & 6) >> 1;
		int tmp_size = (int) *pointer >> 3 << 3;
		int tmp_next_ID = (int) (*(pointer + tmp_size / 8) & 6) >> 1;
		int tmp_next_size = (int) *(pointer + tmp_size / 8) >> 3 << 3;
		if (tmp_flag == 0){
			if (tmp_next_ID == tmp_ID){
				*pointer = (tmp_next_size + tmp_size) | tmp_ID << 1;
				pointer += (tmp_next_size + tmp_size) / 8 - 1;
				*pointer = (tmp_next_size + tmp_size) | tmp_ID << 1;
				pointer++;
				num_of_blocks--;
			}
			else
				pointer += tmp_size / 8;
		}
		else
			pointer += tmp_size / 8;
	}
	
	/* Now we can add the blank block at the end of the memory */
	
	pointer = (uint64_t*) ram;
	ID = (int) (*pointer & 6) >> 1;

	while(ID != 0){
		int tmp_size = *pointer >> 3 << 3;
		pointer += tmp_size / 8;
		ID = (int) (*pointer & 6) >> 1;
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

void sort(uint64_t* tmp_buf, uint64_t* block_start, int tot_size){
	int i = 0;
	int swap_made = 0;
	uint64_t* pointer = block_start;
	while(1){
		int size = *pointer >> 3 << 3;
		if ((*(pointer + size / 8) >> 3 << 3) < size){
			swap_adj(tmp_buf, pointer, pointer + size / 8);
			pointer += size / 8;
			i += size;
			swap_made = 1;
		}
		else{
			pointer += size / 8;
			i += size;
		}
		if (size > tot_size && !swap_made){
			break;
		}
		if (size > tot_size){
			pointer = block_start;
			size = 0;
		}
	}
	return;
}

void swap_adj(uint64_t* tmp_buf, uint64_t* first_block, uint64_t* second_block){
	int size1 = *first_block >> 3 << 3;
	int size2 = *second_block >> 3 << 3;
	if (&first_block < &second_block){
		memcpy(tmp_buf, first_block, size1);
		memcpy(first_block, second_block, size2);
		memcpy(first_block + size2 / 8, tmp_buf, size1);
	}
	else{
		memcpy(tmp_buf, second_block, size2);
		memcpy(second_block, first_block, size1);
		memcpy(second_block + size1 / 8, tmp_buf, size2);
	}
	return;
}
