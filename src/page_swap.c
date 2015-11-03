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

	int least_used_page;
	PageAlgorithmResults* pageResults = NULL;

	for(int i=0; i<frameTable.size; i++){
		if(frameTable.entries[i].pageTableIdx == pageNumber){
			return pageResults;
		}
		
	}

	if(!dyn_array_extract_front(frameIdxList, &least_used_page)){
		return NULL;
	}
	if(!dyn_array_push_back(frameIdxList, &pageNumber)){
		return NULL;
	}

	if(!write_to_back_store(blockStore, frameTable, &least_used_page)){
		return NULL;
	}

	if(!read_from_back_store(blockStore, frameTable, pageNumber)){
		return NULL;
	}

	pageResults->pageRequested = pageNumber;
	pageResults->frameReplaced = least_used_page;
	pageResults->pageReplaced = least_used_page;


	return pageResults;
}

PageAlgorithmResults* approx_least_recently_used (const uint32_t pageNumber, const size_t timeInterval) {	
	PageAlgorithmResults* pageResults = NULL;
	return pageResults;
}

bool read_from_back_store (block_store_t* blockStore, FrameTable_t frameTable, uint32_t pageNumber) {
	if(block_store_read(blockStore, pageNumber, &frameTable, sizeof(frameTable), 0) == 0){
	 	return false;
	 }

	 return true;
}

bool write_to_back_store (block_store_t* blockStore, FrameTable_t frameTable, uint32_t pageNumber) {

	 if(block_store_write(blockStore, pageNumber, &frameTable, sizeof(frameTable), 0) == 0){
	 	return false;
	 }

	 return true;
}

dyn_array_t* read_page_requests ( const char* const filename) {

	FILE *prFile = fopen(filename,"rb");
	int i, numPages[1];
	
	fread(numPages, sizeof(int), 1, prFile);

	int size = numPages[0];
	dyn_array_t* page_requests = malloc(size*sizeof(int)+1);
	int prData[size]; 
	
	page_requests = dyn_array_create(size ,size*sizeof(int)+1,NULL);
	int returnArray[size];

	for(i=0; i<size; i++){

			fread(&prData, sizeof(int), 1, prFile);
			
			returnArray[i] = prData[0];
			}
	
	page_requests = dyn_array_import(returnArray, size, sizeof(int), NULL);
	
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
		block_store_request(blockStore, i);


		if(!dyn_array_insert(frameIdxList, i, &i)){
			return NULL;
		}	
	
	}
	pageTable.size = 512;


	/* Fill the entire Frame Table with correct values*/
	
	for (int i = 0; i < 512; ++i) {
		Frame_t* newFrame = malloc(sizeof(Frame_t));
		newFrame->pageTableIdx = i;
		frameTable.entries[i] = *newFrame; 
	}

	frameTable.size = 512;

		if(pageTable.size == 0 || frameTable.size == 0)
			return false;

	return true;
	
}
