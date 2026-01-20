#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zip.h>
#include <errno.h>

#include "readzip.h"

int main()
{
    const char *archive_path = "hgt_files/N41W122.SRTMGL3.hgt.zip"; // Replace with your zip file path
    const char *target_file = "N41W122.hgt";    // Replace with the file name inside the zip

    char *file_content = read_file_from_zip(archive_path, target_file);

    char number[2];
    int h;
    if (file_content != NULL)
    {
        printf("checking position 1162585 (middle of file):\n");
        // Copy the first two bytes into number array
        number[0] = file_content[1162585 * 2];     // High byte
        number[1] = file_content[1162585 * 2 + 1];

        // Print each byte's hexadecimal value
        for (int i = 0; i < 2; i++) {
            // %02X ensures a minimum width of 2 characters and uses leading zeros
            printf("Byte %d value: 0x%02X\n", i + 1, number[i]);
        }
        
        // convert from big-endian to little-endian
        h = number[1];
        if (h < 0)
        {
            h = h + 255;
        }
        h += number[0] << 8;
        fprintf(stdout, "examined position %d in tile %s, calced little-endianheight %f\n", 1162585, target_file, (float)h);

        free(file_content); // Free the allocated memory
    }

    return 0;
}
