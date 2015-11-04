#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>


#include <dyn_array.h>
#include <block_store.h>

#include "../include/page_swap.h"

/*
 * Global protected variables
 **/
static FrameTable_t frameTable;
static PageTable_t pageTable;
static dyn_array_t* frameIdxList;
static block_store_t* blockStore;

/*
 * CLEARS THE PAGE TABLE AND FRAME TABLE WITH ALL ZEROS
 **/
bool initialize_back_store (void) {
	blockStore = block_store_create();
	if (!blockStore) {
		return false;
	}
	return true;
}

void destroy_back_store(void) {
	block_store_destroy(blockStore, BS_NO_FLUSH);
}
bool initailize_frame_list(void) {
	frameIdxList = dyn_array_create(512,sizeof(unsigned int),NULL);
	if (!frameIdxList) {
		return false;
	}
	return true;
}
void destroy_frame_list(void) {
	dyn_array_destroy(frameIdxList);
}


 
PageAlgorithmResults* least_recently_used(const uint32_t pageNumber) {

	int least_used_page_idx, found_page_idx;

	//Allocate memory to our return struct
	PageAlgorithmResults* pageResults = malloc (sizeof (PageAlgorithmResults));
	for(int i=0; i<pageTable.size; i++){
		if(pageTable.entries[i].frame == pageNumber){//loop through page table to find passed page and
			if(!dyn_array_extract(frameIdxList, pageNumber, &found_page_idx)){ //if he page is found take it out of it's position in the array and push it to the back(equal to top on a stack)
				return NULL;	
				}																
																								
			if(!dyn_array_push_back(frameIdxList, &found_page_idx)){
				return NULL;
			}

			return pageResults = NULL;//exit from functino since page is found
		}
	}
	//Part of code only reachable if page not found, 
	if(!dyn_array_extract_front(frameIdxList, &least_used_page_idx)){ //pull victim frame index from top of the stack
		return NULL;
	}

	if(!write_to_back_store(blockStore, &frameTable.entries[least_used_page_idx], least_used_page_idx)){//save victime frame to back store
		return NULL;
	}


	if(!read_from_back_store(blockStore, &frameTable.entries[least_used_page_idx], pageNumber)){//read in the requested page from the back store
		return NULL;
	}

	if(!dyn_array_push_back(frameIdxList, &frameTable.entries[least_used_page_idx])){//Push that index to the back (equal to bottom of stack)
		return NULL;
	} 

	pageResults->pageRequested = pageNumber;
	pageResults->frameReplaced = least_used_page_idx;
	pageResults->pageReplaced = frameTable.entries[least_used_page_idx].pageTableIdx;



	return pageResults;
}

PageAlgorithmResults* approx_least_recently_used (const uint32_t pageNumber, const size_t timeInterval) {	
	
	PageAlgorithmResults* pageResults = malloc (sizeof (PageAlgorithmResults));
	int min_bit_idx;


	for(int i=0; i<pageTable.size; i++){//loop through page table to see if page is present
		if(pageTable.entries[i].frame == pageNumber){
			return pageResults = NULL;//immediately exit if found
		}
	}
	
	unsigned char minVal = frameTable.entries[0].accessTrackingByte;
	
	for(int i=0; i<frameTable.size; i++){

		int result = memcmp(&frameTable.entries[i].accessTrackingByte, &minVal, sizeof(unsigned char));//FInd the frame with the minimum acessTrackingByte to use as the victim frame
			if(result < 0){
				min_bit_idx = i;
				minVal = frameTable.entries[i].accessTrackingByte;
			}
	}
	

	pageResults->pageRequested = pageNumber;			//store results 
	pageResults->frameReplaced = min_bit_idx;
	pageResults->pageReplaced = frameTable.entries[min_bit_idx].pageTableIdx;

	if(!write_to_back_store(blockStore, &frameTable.entries[min_bit_idx], min_bit_idx)){//save victim frame in back store
		return NULL;
	}


	if(!read_from_back_store(blockStore, &frameTable.entries[min_bit_idx], pageNumber)){//read in the newly requested frame
		return NULL;
	}
		frameTable.entries[min_bit_idx].accessBit |= (1<<0);//set accessBit
		
	


	if(timeInterval % 100 == 0){//Every 100 iterations shift the tracking byte to the right one and set the value of the access bit to that tracking byte's most significant bit
		for(int i=0; i<frameTable.size; i++){
			frameTable.entries[i].accessTrackingByte = frameTable.entries[i].accessTrackingByte >> 1;
			unsigned char bit = (frameTable.entries[i].accessBit >> 0) & 1;//getting the 1 or 0 contained in the accessBit
  			frameTable.entries[i].accessTrackingByte |= (bit << 7); //PLace that bit in the msb
		}
	}
	
	return pageResults;
	
}

bool read_from_back_store (block_store_t* blockStore, Frame_t *frame, uint32_t pageNumber) {
	if(block_store_read(blockStore, pageNumber, frame, sizeof(frame), 8) == 0){
	 	return false;
	 }

	 return true;
}

bool write_to_back_store (block_store_t* blockStore, Frame_t *frame, uint32_t pageNumber) {
	 if(block_store_write(blockStore, pageNumber, frame, sizeof(frame), 8) == 0){
	 	return false;

	 }
	 return true;

}

dyn_array_t* read_page_requests ( const char* const filename) {

	FILE *prFile = fopen(filename,"rb");
	int i, numPages[1];											//Allocate memory, valgrind hates this entire function but still reads the binary file
	
	fread(numPages, sizeof(int), 1, prFile);//Get size of file

	int size = numPages[0];
	dyn_array_t* page_requests = malloc(size*sizeof(int)+1);
	int prData[size]; 
	
	page_requests = dyn_array_create(size ,size*sizeof(int)+1,NULL);
	int returnArray[size];

	for(i=0; i<size; i++){//fill array with file contents

			fread(&prData, sizeof(int), 1, prFile);
			
			returnArray[i] = prData[0];
			}
	
	page_requests = dyn_array_import(returnArray, size, sizeof(int), NULL);//import that array into page requests

	fclose(prFile);
	
	return page_requests;
}

bool initialize (void) {
	
	/*zero out my tables*/
	memset(&frameTable,0,sizeof(FrameTable_t));
	memset(&pageTable,0,sizeof(PageTable_t));

	/* Fill the Page Table from 0 to 512*/
	for (int i = 0; i < 512; ++i) {
		Page_t newPage;
		newPage.frame = i;
		newPage.valid = 1;
		pageTable.entries[i] = newPage;
		pageTable.size++;

	
		frameTable.entries[i].pageTableIdx= i;
		frameTable.entries[i].accessBit = 0;
		frameTable.entries[i].accessBit = (unsigned char)malloc(sizeof(unsigned char)*sizeof(int));
		frameTable.entries[i].accessTrackingByte = 0;
		frameTable.entries[i].accessTrackingByte = (unsigned char)malloc(sizeof(unsigned char)*sizeof(int));
		frameTable.size++;
		
		if(block_store_read(blockStore, i, &frameTable.entries[i].data, sizeof(Frame_t), 8)){
			return false; //fill frame data with bogus data
		}
			
		if(!dyn_array_insert(frameIdxList, i, &i)){//Fill up frameIdxList to use for LRU
			return false;
		}

		
	}

	return true;
	
}
