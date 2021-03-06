#ifndef _PAGE_SWAP_H_
#define _PAGE_SWAP_H_
#include <block_store.h>
#include <stdbool.h>

#define MAX_PAGE_TABLE_ENTRIES_SIZE 2048
#define MAX_PHYSICAL_MEMORY_SIZE 512

typedef struct {
	unsigned int pageTableIdx;
	unsigned char data[1024];
	unsigned char accessTrackingByte;
	unsigned char accessBit;
}Frame_t;

typedef struct {
	Frame_t entries[MAX_PHYSICAL_MEMORY_SIZE];
	unsigned int size;
}FrameTable_t;


// TODO CREATE A Page_t Structure
typedef struct{
	unsigned short frame; // frame index/address
	int valid;
}Page_t;

typedef struct{
	// An array of pages
	Page_t entries[MAX_PAGE_TABLE_ENTRIES_SIZE];
	// Number of Page entires
	unsigned int size;
}PageTable_t;


typedef struct {
	unsigned short pageRequested;
	unsigned short frameReplaced;
	unsigned short pageReplaced;
}PageAlgorithmResults;


bool initialize_back_store (void);

void destroy_back_store(void);

bool initailize_frame_list(void);

void destroy_frame_list(void);

PageAlgorithmResults* least_recently_used(const uint32_t pageNumber);

PageAlgorithmResults* approx_least_recently_used (const uint32_t pageNumber, const size_t timeInterval);

bool read_from_back_store (block_store_t* blockStore, Frame_t *frame, uint32_t pageNumber);

bool write_to_back_store (block_store_t* blockStore, Frame_t *frame, uint32_t pageNumber);

dyn_array_t* read_page_requests (const char* const filename);

bool initialize (void); 

#endif
