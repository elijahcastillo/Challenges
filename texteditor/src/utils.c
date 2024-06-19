#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

void logMessage(const char *format, ...) {
    FILE *logFile = fopen("editor.log", "a");
    if (logFile == NULL) {
        perror("Failed to open log file");
        return;
    }

    va_list args;
    va_start(args, format);
    vfprintf(logFile, format, args);
    va_end(args);

    fprintf(logFile, "\n");
    fclose(logFile);
}

// Function to read a file into a string
char* readFileToString(const char *filename) {
    // Open the file in read mode ("r")
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        // If the file can't be opened, print an error message and return NULL
        perror("Error opening file");
        return NULL;
    }

    // Seek to the end of the file to determine its size
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    // Seek back to the beginning of the file
    fseek(file, 0, SEEK_SET);

    // Allocate memory to store the file content + null terminator
    char *buffer = (char*)malloc((fileSize + 1) * sizeof(char));
    if (buffer == NULL) {
        // If memory allocation fails, print an error message, close the file, and return NULL
        perror("Error allocating memory");
        fclose(file);
        return NULL;
    }

    // Read the file content into the buffer
    size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
    if (bytesRead != fileSize) {
        // If the number of bytes read doesn't match the file size, print an error message, free the buffer, close the file, and return NULL
        perror("Error reading file");
        free(buffer);
        fclose(file);
        return NULL;
    }

    // Null-terminate the string
    buffer[fileSize] = '\0';

    // Close the file
    fclose(file);

    // Return the buffer containing the file content
    return buffer;
}

