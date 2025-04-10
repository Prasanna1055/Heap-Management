#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

typedef struct metadata
{
    struct metadata *next;
    int size;
} metadata;

char memory[10000]; 
metadata *freeList = (metadata *)memory;


typedef struct allocated_block {
    void *ptr;
    int id;
    int size;
    struct allocated_block *next;
} allocated_block;

allocated_block *allocatedList = NULL;
int nextBlockId = 1;

void addToAllocatedList(void *ptr, int size) {
    allocated_block *newBlock = (allocated_block *)malloc(sizeof(allocated_block));
    newBlock->ptr = ptr;
    newBlock->id = nextBlockId++;
    newBlock->size = size;
    newBlock->next = allocatedList;
    allocatedList = newBlock;
}


int removeFromAllocatedList(void *ptr) {
    allocated_block *curr = allocatedList;
    allocated_block *prev = NULL;
    
    while (curr != NULL) {
        if (curr->ptr == ptr) {
            if (prev == NULL) {
                allocatedList = curr->next;
            } else {
                prev->next = curr->next;
            }
            int id = curr->id;
            free(curr);
            return id;
        }
        prev = curr;
        curr = curr->next;
    }
    return -1;  
}

void merge(metadata *prev)
{
    if (prev == NULL) return;
    
    metadata *curr = prev->next;
    if (curr == NULL) return;

    if (curr->next != NULL && (char *)curr + curr->size + sizeof(metadata) == (char *)curr->next)
    {
        curr->size = curr->size + curr->next->size + sizeof(metadata);
        curr->next = curr->next->next;
    }

    if ((char *)prev + prev->size + sizeof(metadata) == (char *)curr)
    {
        prev->size = prev->size + curr->size + sizeof(metadata);
        prev->next = curr->next;
    }
}

void deallocate(void *ptr)
{
    if (ptr == NULL)
    {
        printf("\nInvalid block");
        return;
    }
    
    metadata *block = (metadata *)((char *)ptr - sizeof(metadata));
    metadata *temp = freeList;
    metadata *prev = NULL;

    while (temp != NULL && temp < block)
    {
        prev = temp;
        temp = temp->next;
    }

    if (prev == NULL)
    {
        block->next = freeList;
        freeList = block;
    }
    else
    {
        block->next = prev->next;
        prev->next = block;
    }
    
    printf("\n\nBefore merging: ");
    temp = freeList;
    while (temp != NULL)
    {
        printf("\naddress: %p, size: %d", temp, temp->size);
        temp = temp->next;
    }

    if (prev == NULL)
    {
        if (freeList->next != NULL)
            merge(freeList);
    }
    else
    {
        merge(prev);
    }
    
    printf("\n\nAfter Merging: ");
    temp = freeList;
    while (temp != NULL)
    {
        printf("\naddress: %p, size: %d", temp, temp->size);
        temp = temp->next;
    }
    
    int blockId = removeFromAllocatedList(ptr);
    if (blockId != -1) {
        printf("\nDeallocated block #%d", blockId);
    } else {
        printf("\nDeallocated unknown block");
    }
}

void *allocate(int blockSize)
{
    if (blockSize <= 0)
    {
        printf("\nInvalid block size");
        return NULL;
    }
    
    metadata *curr = freeList;
    metadata *bestBlock = NULL;
    metadata *bestPrev = NULL;
    metadata *prev = NULL;
    int bestSize = INT_MAX;

    while (curr != NULL)
    {
        if (curr->size >= blockSize && curr->size < bestSize)
        {
            bestBlock = curr;
            bestPrev = prev;
            bestSize = curr->size;

            if (curr->size == blockSize)
                break;
        }
        prev = curr;
        curr = curr->next;
    }
    
    if (bestBlock == NULL)
    {
        printf("\nNo memory available");
        return NULL;
    }
    
    void *userBlock;

    if (bestBlock->size < blockSize + sizeof(metadata) + 1)
    {
        if (bestPrev == NULL)
        {
            freeList = bestBlock->next;
        }
        else
        {
            bestPrev->next = bestBlock->next;
        }
        
        userBlock = (char *)bestBlock + sizeof(metadata);
        printf("\n\nAllocated entire block ");
    }
    else
    {
        metadata *newBlock = (metadata *)((char *)bestBlock + sizeof(metadata) + blockSize);
        newBlock->size = bestBlock->size - blockSize - sizeof(metadata);
        newBlock->next = bestBlock->next;
        
        if (bestPrev == NULL)
        {
            freeList = newBlock;
        }
        else
        {
            bestPrev->next = newBlock;
        }
        
        bestBlock->size = blockSize;
        userBlock = (char *)bestBlock + sizeof(metadata);
        printf("\n\nAllocated with split ");
    }
    
    addToAllocatedList(userBlock, blockSize);
    printf("- Block #%d (size: %d, address: %p)", nextBlockId-1, blockSize, userBlock);
    
    return userBlock;
}

void displayMenu() {
    printf("\n\n==== Memory Allocator Menu ====");
    printf("\n1. Allocate memory");
    printf("\n2. Deallocate memory");
    printf("\n3. Show allocated blocks");
    printf("\n4. Show free list");
    printf("\n5. Exit");
    printf("\nEnter your choice: ");
}

void showAllocatedBlocks() {
    allocated_block *curr = allocatedList;
    
    if (curr == NULL) {
        printf("\nNo blocks are currently allocated.");
        return;
    }
    
    printf("\n\n===== Allocated Blocks =====");
    while (curr != NULL) {
        printf("\nBlock #%d - Size: %d bytes, Address: %p", 
               curr->id, curr->size, curr->ptr);
        curr = curr->next;
    }
}

void showFreeList() {
    metadata *temp = freeList;
    
    if (temp == NULL) {
        printf("\nFree list is empty.");
        return;
    }
    
    printf("\n\n===== Free Memory Blocks =====");
    int totalFree = 0;
    int blockCount = 0;
    
    while (temp != NULL) {
        printf("\nBlock #%d - Size: %d bytes, Address: %p", 
               ++blockCount, temp->size, temp);
        totalFree += temp->size;
        temp = temp->next;
    }
    
    printf("\n\nTotal free memory: %d bytes in %d blocks", totalFree, blockCount);
}


void* findBlockById(int id) {
    allocated_block *curr = allocatedList;
    while (curr != NULL) {
        if (curr->id == id) {
            return curr->ptr;
        }
        curr = curr->next;
    }
    return NULL;
}

int main()
{
    
    freeList->next = NULL;  
    freeList->size = sizeof(memory) - sizeof(metadata); 
    
    printf("\nMemory Allocator Initialized");
    printf("\nTotal memory: %lu bytes", sizeof(memory));
    printf("\nInitial free memory: %d bytes\n", freeList->size);
    
    int choice;
    int size;
    void *ptr;
    int blockId;
    char buffer[10];
    
    do {
        displayMenu();
        if (scanf("%d", &choice) != 1) {
    
            while (getchar() != '\n');
            printf("\nInvalid input. Please enter a number.");
            continue;
        }
        
        while (getchar() != '\n');
        
        switch (choice) {
            case 1: 
                printf("\nEnter size to allocate (in bytes): ");
                if (scanf("%d", &size) != 1) {
                    printf("\nInvalid size input.");
                    while (getchar() != '\n');
                    break;
                }
                while (getchar() != '\n'); 
                
                ptr = allocate(size);
                if (ptr) {
                    memset(ptr, 'A', size);
                }
                break;
                
            case 2: 
                if (allocatedList == NULL) {
                    printf("\nNo blocks are currently allocated.");
                    break;
                }
                
                showAllocatedBlocks();
                printf("\nEnter block ID to deallocate: ");
                if (scanf("%d", &blockId) != 1) {
                    printf("\nInvalid ID input.");
                    while (getchar() != '\n');
                    break;
                }
                while (getchar() != '\n'); 
                
                ptr = findBlockById(blockId);
                if (ptr != NULL) {
                    deallocate(ptr);
                } else {
                    printf("\nBlock with ID %d not found.", blockId);
                }
                break;
                
            case 3: 
                showAllocatedBlocks();
                break;
                
            case 4: 
                showFreeList();
                break;
                
            case 5: 
                printf("\nExiting program.\n");
                break;
                
            default:
                printf("\nInvalid choice. Please try again.");
        }
    } while (choice != 5);

    allocated_block *curr = allocatedList;
    while (curr != NULL) {
        allocated_block *temp = curr;
        curr = curr->next;
        free(temp);
    }
    
    return 0;
}