#ifndef TILES_H
#define TILES_H
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define SRTM_TILE_SIZE 1201 * 1201              // SRTMGL3 tiles are 1201x1201 samples
#define SRTM_TILE_SIZE_BYTES SRTM_TILE_SIZE * 2 // Example size for height data (SRTMGL3), short ints, 2 bytes each

// Define the struct with a pointer member
typedef struct
{
    int id;                                    // the index of the tile
    bool is_available;                         // whether the tile is available from NASA
    char *name;                                // filename of the tile
    char heightData[SRTM_TILE_SIZE_BYTES + 1]; // height data array plus null terminator
} Tile;

// Structure to manage the dynamic array of Tile structs
typedef struct
{
    Tile *array;
    size_t size;
    size_t capacity;
} TileArray;

// Function to initialize the dynamic array
void init_TileArray(TileArray *ta, size_t initialCapacity);

// Function to add a new tile to the dynamic array
void add_Tile(TileArray *ta, int id, bool is_available, const char *name, const char *heightData);

// Function to free all allocated memory
void free_TileArray(TileArray *ta);

// Function to list all tiles
void listTiles(TileArray *ta);

Tile *find_tile_by_id(TileArray *ta, int target_id);

Tile *find_tile_by_name(TileArray *ta, const char *target_name);
#endif // TILES_H