#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zip.h>
#include <errno.h>

#include "readzip.h"

int main()
{
    const char *archive_path = "hgt_files/N44W070.SRTMGL3.hgt.zip"; // Replace with your zip file path
    const char *target_file = "N44W070.hgt";    // Replace with the file name inside the zip

    char *file_content = read_file_from_zip(archive_path, target_file);

    if (file_content != NULL)
    {
        printf("Content of %s:\n%s\n", target_file, file_content);
        free(file_content); // Free the allocated memory
    }

    return 0;
}
