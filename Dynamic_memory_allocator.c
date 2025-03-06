#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BATCH 10
#define FREE '_'
#define STOP '@'
#define REMOVE 8

// Structure to represent memory blocks
struct space {
    char *memory;
    int len;
};

// Structure for the linked list of allocated memory blocks
struct node {
    int p;
    struct node *next;
};

// Function to initialize memory
void initialize(struct space *mem) {
    mem->memory = (char *)malloc(BATCH + 1);  // Allocate BATCH + 1 bytes
    mem->len = BATCH;
    memset(mem->memory, FREE, mem->len);  // Fill memory with FREE space
    mem->memory[mem->len] = '\0';  // Null terminate the memory

    printf("memory=%s(%d)\n", mem->memory, mem->len);
}

// Function to clear memory
void clear(struct space *mem) {
    printf("memory=%s(%d)\n", mem->memory, mem->len);
    free(mem->memory);  // Free the allocated memory
}

// Function to scan memory for free blocks
int scanMemory(struct space *mem, int size) {
    int p = -1;
    int i = 0;

    while (p == -1 && i < mem->len) {
        if (mem->memory[i] == FREE) {
            int t = 0;
            while (mem->memory[i + t] == FREE && t < size) {
                t++;
            }
            if (t == size) {
                p = i;
            } else {
                i += t;
            }
        } else {
            i++;
        }
    }

    return p;
}

// Function to add a node to the linked list
void addNode(struct node **head, int p) {
    struct node *new_node = (struct node *)malloc(sizeof(struct node));
    new_node->p = p;
    new_node->next = *head;
    *head = new_node;
}

// Allocator function to allocate memory for a word
int allocator(struct space *mem, int size, struct node **head) {
    int p = scanMemory(mem, size);

    if (p >= 0) {
        addNode(head, p);  // Add the allocated block to the linked list
    }

    return p;
}

// Function to remove a node from the linked list
void removeNode(struct node **head, int p) {
    struct node *cur = *head;
    struct node *prev = NULL;

    while (cur != NULL) {
        if (cur->p == p) {
            if (prev != NULL) {
                prev->next = cur->next;
            } else {
                *head = cur->next;
            }
            free(cur);  // Free the node
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

// Function to deallocate memory for a word
void deallocator(struct space *mem, int p, struct node **head) {
    int i = p;
    while (mem->memory[i] != STOP) {
        mem->memory[i] = FREE;  // Mark memory as FREE
        i++;
    }
    mem->memory[i] = FREE;  // Mark STOP character as FREE

    printf("memory=%s(%d)\n", mem->memory, mem->len);

    removeNode(head, p);  // Remove the allocated block from the linked list
}

// Function to expand memory
void addMemory(struct space *mem) {
    char *new_memory = (char *)malloc(mem->len + BATCH + 1);  // Allocate additional memory
    int i;

    // Copy old memory content
    for (i = 0; i < mem->len; i++) {
        new_memory[i] = mem->memory[i];
    }

    // Mark new memory as FREE
    for (i = mem->len; i < mem->len + BATCH; i++) {
        new_memory[i] = FREE;
    }

    new_memory[mem->len + BATCH] = '\0';  // Null terminate the memory

    free(mem->memory);  // Free old memory
    mem->memory = new_memory;  // Update memory pointer
    mem->len = mem->len + BATCH;  // Update memory length

    printf("memory=%s(%d)\n", mem->memory, mem->len);
}

// New allocator function that expands memory if necessary
int allocatorNew(struct space *mem, int size, struct node **head) {
    int p = -1;
    
    while (p == -1) {
        p = scanMemory(mem, size);

        if (p == -1) {
            addMemory(mem);  // Expand memory if no block of the required size is found
        }
    }

    addNode(head, p);  // Add the allocated block to the linked list

    return p;
}

