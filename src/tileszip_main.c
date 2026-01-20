
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tiles.h"
#include "readzip.h"

int main()
{
    TileArray ta;
    int h;
    char number[2];

    init_TileArray(&ta, 2); // Initial capacity of 2

    const char *archive_path = "hgt_files/N41W122.SRTMGL3.hgt.zip"; // Replace with your zip file path
    const char *target_file = "N41W122.hgt";                        // Replace with the file name inside the zip

    char *file_content = read_file_from_zip(archive_path, target_file);

    printf("checking position 1162585 (middle of buffer):\n");
    // Copy the first two bytes into number array
    number[0] = file_content[1162585 * 2]; // High byte
    number[1] = file_content[1162585 * 2 + 1];

    // Print each byte's hexadecimal value
    for (int i = 0; i < 2; i++)
    {
        // %02X ensures a minimum width of 2 characters and uses leading zeros
        printf("Byte %d value: 0x%02X\n", i + 1, number[i]);
    }

    h = number[1];
    if (h < 0)
    {
        h = h + 255;
    }
    h += number[0] << 8;
    fprintf(stdout, "examined position %d in buffer of %s, calced little-endianheight %f\n", 1162585, target_file, (float)h);

    add_Tile(&ta, 101, target_file, file_content);
    add_Tile(&ta, 102, "test", file_content);
    add_Tile(&ta, 103, "Charlie", file_content); // This will trigger reallocation

    // Print elements
    for (size_t i = 0; i < ta.size; i++)
    {
        printf("Tile ID: %d, Name: %s\n", ta.array[i].id, ta.array[i].name);
    }

    listTiles(&ta);

    Tile *found = find_tile_by_id(&ta, 101);
    if (found != NULL)
    {
        printf("Found Tile ID: %d, Name: %s\n", found->id, found->name);
        // Optionally print height data samples

        printf("checking position 1162585 (middle of file):\n");
        // Copy the first two bytes into number array
        number[0] = found->heightData[1162585 * 2]; // High byte
        number[1] = found->heightData[1162585 * 2 + 1];

        // Print each byte's hexadecimal value
        for (int i = 0; i < 2; i++)
        {
            // %02X ensures a minimum width of 2 characters and uses leading zeros
            printf("Byte %d value: 0x%02X\n", i + 1, number[i]);
        }

        h = number[1];
        if (h < 0)
        {
            h = h + 255;
        }
        h += number[0] << 8;
        fprintf(stdout, "examined position %d in tile %s, calced little-endianheight %f\n", 1162585, target_file, (float)h);
    }
    else
    {
        printf("Tile with ID 101 not found.\n");
    }

    Tile *found2 = find_tile_by_id(&ta, 104);
    if (found2 != NULL)
    {
        printf("Found Tile ID: %d, Name: %s\n", found2->id, found2->name);
    }
    else
    {
        printf("Tile with ID 104 not found.\n");
        printf("Adding Tile Dawn with ID 104.\n");
        add_Tile(&ta, 104, "Dawn", file_content);
    }
    listTiles(&ta);

    Tile *found3 = find_tile_by_name(&ta, "Charlie");
    if (found3 != NULL)
    {
        printf("Found Tile ID: %d, Name: %s\n", found3->id, found3->name);
    }
    else
    {
        printf("Tile with name Charlie not found.\n");
    }

    Tile *found4 = find_tile_by_name(&ta, "Edwin");
    if (found4 != NULL)
    {
        printf("Found Tile ID: %d, Name: %s\n", found4->id, found4->name);
    }
    else
    {
        printf("Tile with name Edwin not found.\n");
    }

    add_Tile(&ta, 105, "Edwin", file_content); // This will trigger reallocation
    listTiles(&ta);

    free_TileArray(&ta); // Clean up memory

    return 0;
}
