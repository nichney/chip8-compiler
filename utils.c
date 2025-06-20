#include <string.h>
#include <stdlib.h>

char* get_filename_for_binary(const char* filename) {
    // Create new filename for binary.
    // Change suffix after latest dot to 'ch8',
    // Or add it if no suffic provided
    const char* last_dot = strrchr(filename, '.');
    size_t base_len;
    if (last_dot) {
        base_len = last_dot - filename;
    } else {
        base_len = strlen(filename);
    }   

    // ".ch8" + '\0' = 5
    char* binary_filename = malloc(base_len + 5); 
    if (!binary_filename) return NULL;

    strncpy(binary_filename, filename, base_len);
    strcpy(binary_filename + base_len, ".ch8");

    return binary_filename;
}


