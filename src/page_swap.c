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

/*
 * TODO COMPLETE ALL THE FOLLOWING FOR EACH FUNCTION: LOGIC and DOCUMENTATION 
 */
 
PageAlgorithmResults* least_recently_used(const uint32_t pageNumber) {

	PageAlgorithmResults* pageResults = NULL;
	return pageResults;
}

PageAlgorithmResults* approx_least_recently_used (const uint32_t pageNumber, const size_t timeInterval) {	
	PageAlgorithmResults* pageResults = NULL;
	return pageResults;
}

bool read_from_back_store (/*?????? TODO: add your parameters*/) {
	return false;
}

bool write_to_back_store (/*???? TODO: add your parameters*/) {
	return false;
}

dyn_array_t* read_page_requests ( const char* const filename) {

	FILE *prFile = fopen(filename,"rb");
	int i, numPages[1];
	
	dyn_array_t* page_requests = NULL;

	fread(numPages, sizeof(int), 1, prFile);

	int *prData = malloc(sizeof(int)*numPages[0]);
	
	page_requests = dyn_array_create(0,sizeof(int),NULL);
	
	int *requests_array = malloc(sizeof(int)*numPages[0]);

	Page_t *newPage = malloc(sizeof(Page_t));

		for(i=0; i<numPages[0]; i++){

			fread(&prData[i], sizeof(prData[i]), 1, prFile);
			requests_array[i] = prData[0];

		}

	page_requests = dyn_array_import(requests_array, numPages[0], sizeof(int), NULL);
	
	return page_requests;
}

bool initialize (void) {
	
	/*zero out my tables*/
	memset(&frameTable,0,sizeof(FrameTable_t));
	memset(&pageTable,0,sizeof(PageTable_t));

	Page_t* newPage = malloc(sizeof(Page_t ));
	/* Fill the Page Table from 0 to 512*/
	for (int i = 0; i < 512; ++i) {
		pageTable.entries[i] = *newPage;
	}
	pageTable.size = 512;

	/* Fill the entire Frame Table with correct values*/
	
	for (int i = 0; i < 512; ++i) {
		Frame_t* newFrame = malloc(sizeof(Frame_t));
		newFrame->pageTableIdx = i;
		frameTable.entries[i] = *newFrame; 
	}
	frameTable.size = 512;

		if(pageTable.entries == NULL || pageTable.size == 0 || frameTable.entries == NULL || frameTable.size == 0)
			return false;

	return true;
	
}
