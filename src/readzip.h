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
char *read_file_from_zip(const char *zip_path, const char *file_in_zip);
