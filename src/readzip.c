#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zip.h>
#include <errno.h>

/**
 * Reads the content of a specific file within a zip archive.
 *
 * @param zip_path The path to the zip archive.
 * @param file_in_zip The name/path of the file inside the zip archive.
 * @return A dynamically allocated string containing the file content, or NULL on failure.
 *         The caller is responsible for freeing the memory.
 */
char *read_file_from_zip(const char *zip_path, const char *file_in_zip)
{
    zip_t *archive = NULL;
    zip_file_t *file = NULL;
    zip_stat_t file_info;
    char *content = NULL;
    int err = 0;

    // 0. Check that zip archive exists
    FILE *test = fopen(zip_path, "rb");
    if (test == NULL)
    {
        fprintf(stderr, "Zip archive %s does not exist: %s\n", zip_path, strerror(errno));
        return NULL;
    }
    fclose(test);

    // 1. Open the zip archive in read-only mode
    archive = zip_open(zip_path, ZIP_RDONLY, &err);
    if (archive == NULL)
    {
        fprintf(stderr, "Failed to open zip archive %s: %s\n", zip_path, zip_strerror(archive));
        return NULL;
    }

    // 2. Locate the file within the archive
    // zip_name_locate returns the index of the file, or -1 if not found
    zip_int64_t file_index = zip_name_locate(archive, file_in_zip, 0);
    if (file_index < 0)
    {
        fprintf(stderr, "File %s not found in the archive %s\n", file_in_zip, zip_path);
        zip_close(archive);
        return NULL;
    }

    // 3. Get file information (especially size)
    if (zip_stat_index(archive, file_index, 0, &file_info) != 0)
    {
        fprintf(stderr, "Failed to get file info for %s: %s\n", file_in_zip, zip_strerror(archive));
        zip_close(archive);
        return NULL;
    }

    // 4. Open the located file for reading
    file = zip_fopen_index(archive, file_index, 0);
    if (file == NULL)
    {
        fprintf(stderr, "Failed to open file %s inside archive: %s\n", file_in_zip, zip_strerror(archive));
        zip_close(archive);
        return NULL;
    }

    // 5. Allocate memory for the file content (+1 for null terminator)
    fprintf(stdout, "readzip Allocating %zu bytes for file content of %s\n", (long unsigned int)(file_info.size + 1), file_in_zip);
    content = (char *)malloc(file_info.size + 1);
    if (content == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        zip_fclose(file);
        zip_close(archive);
        return NULL;
    }

    // 6. Read the file data into the buffer
    zip_int64_t bytes_read = zip_fread(file, content, file_info.size);
    if (bytes_read < (zip_int64_t)file_info.size)
    {
        fprintf(stderr, "Failed to read full content of file %s\n", file_in_zip);
        free(content);
        zip_fclose(file);
        zip_close(archive);
        return NULL;
    }

    // 7. Null-terminate the string
    content[file_info.size] = '\0';

    // 8. Close the file inside the zip and the archive
    zip_fclose(file);
    zip_close(archive);

    return content;
}
