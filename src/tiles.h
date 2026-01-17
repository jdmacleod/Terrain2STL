#include <stdio.h>
#include <stdlib.h>

// Define the struct with a pointer member
typedef struct {
    int id;
    char *name; // Pointer to a dynamically allocated string
    char *heightData; // Pointer to dynamically allocated height data
} Tile;

// Structure to manage the dynamic array of Tile structs
typedef struct {
    Tile *array;
    size_t size;
    size_t capacity;
} TileArray;

// Function to initialize the dynamic array
void init_TileArray(TileArray *ta, size_t initialCapacity);

// Function to add a new tile to the dynamic array
void add_Tile(TileArray *ta, int id, const char *name, const char *heightData);

// Function to free all allocated memory
void free_TileArray(TileArray *ta);

// Function to list all tiles
void listTiles(TileArray *ta);

Tile* find_tile_by_id(TileArray *ta, int target_id);

Tile* find_tile_by_name(TileArray *ta, const char *target_name);