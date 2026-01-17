#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tiles.h"

// Function to initialize the dynamic array
void init_TileArray(TileArray *ta, size_t initialCapacity) {
    ta->array = (Tile *)malloc(initialCapacity * sizeof(Tile));
    if (ta->array == NULL) {
        perror("Initial allocation failed");
        exit(EXIT_FAILURE);
    }
    ta->size = 0;
    ta->capacity = initialCapacity;
}

// Function to add a new tile to the dynamic array
void add_Tile(TileArray *ta, int id, const char *name, const char *heightData) {
    // 1. Check Capacity and Reallocate
    if (ta->size >= ta->capacity) {
        size_t newCapacity = ta->capacity * 2; // Double capacity (common practice)
        // Use a temporary pointer for safe reallocation
        Tile *tmp = (Tile *)realloc(ta->array, newCapacity * sizeof(Tile));
        if (tmp == NULL) {
            perror("Reallocation failed");
            // Free the original array before exiting to avoid a memory leak
            free(ta->array); 
            exit(EXIT_FAILURE);
        }
        ta->array = tmp;
        ta->capacity = newCapacity;
        printf("Resized array to capacity %zu\n", newCapacity);
    }

    // 2. Allocate memory for the internal pointer (the 'name' string)
    // You must allocate enough space for the string and the null terminator
    ta->array[ta->size].name = (char *)malloc((strlen(name) + 1) * sizeof(char));
    if (ta->array[ta->size].name == NULL) {
        perror("Name allocation failed");
        // Handle error: possibly free other memory and exit
        exit(EXIT_FAILURE);
    }
    ta->array[ta->size].heightData = (char *)malloc((strlen(heightData) + 1) * sizeof(char));
    if (ta->array[ta->size].heightData == NULL) {
        perror("Height data allocation failed");
        // Handle error: possibly free other memory and exit
        exit(EXIT_FAILURE);
    }
    
    // 3. Assign values
    ta->array[ta->size].id = id;
    strcpy(ta->array[ta->size].name, name); // Copy the string

    strcpy(ta->array[ta->size].heightData, heightData); // Copy the height data string

    // 4. Update size
    ta->size++;
}

// Function to free all allocated memory
void free_TileArray(TileArray *ta) {
    // Free the dynamically allocated 'name' and 'heightData' strings inside each struct
    for (size_t i = 0; i < ta->size; i++) {
        free(ta->array[i].name);
        free(ta->array[i].heightData);
    }
    // Free the array of structs itself
    free(ta->array);
    ta->array = NULL;
    ta->size = 0;
    ta->capacity = 0;
}

void listTiles(TileArray *ta) {
    for (size_t i = 0; i < ta->size; i++) {
        printf("listing Tile ID: %d, Name: %s, Height Data: %s\n", ta->array[i].id, ta->array[i].name, ta->array[i].heightData);
    }
}

Tile* find_tile_by_id(TileArray *ta, int target_id) {
    for (size_t i = 0; i < ta->size; i++) {
        printf("listing Tile ID: %d, Name: %s, Height Data: %s\n", ta->array[i].id, ta->array[i].name, ta->array[i].heightData);

        // Compare the 'id' member of the current struct with the target id
        if (ta->array[i].id == target_id) {
            return &ta->array[i]; // Return a pointer to the found struct
        }
    }
    return NULL; // Return NULL if not found
}

Tile* find_tile_by_name(TileArray *ta, const char *target_name) {
    for (size_t i = 0; i < ta->size; i++) {
        printf("listing Tile ID: %d, Name: %s, Height Data: %s\n", ta->array[i].id, ta->array[i].name, ta->array[i].heightData);

        // Compare the 'name' member of the current struct with the target name
        if (strcmp(ta->array[i].name, target_name) == 0) {
            return &ta->array[i]; // Return a pointer to the found struct
        }
    }
    return NULL; // Return NULL if not found
}
