/*
 * HW-1 - bbcp.c - bare-bones copy a file
 * Author: Devharsh Trivedi
 * Email: dtrived5@stevens.edu
*/

#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


/* limit for a file name */
#ifndef BUF_LIMIT
#define BUF_LIMIT 512
#endif

void handleError(char *msg) {
    perror(msg);
    exit(1);
}

/*
 * Opens a file and returns a valid file ptr, in case of an error,
 * calls the errorHandle function so the program exits
 */
FILE* openFile(const char* path, const char* mode){
    FILE* file = fopen(path, mode);
    if(file == NULL) {
        handleError("Error opening file!\n");
    }
    return file;
}

/*
 * Closes the file and returns, calls handleError in case the file
 * is not closed properly.
 */
void closeFile(FILE* ptr){
    if(fclose(ptr) == -1) {
        handleError("Error closing file!\n");
    }
}

void resolveLink(char* path){
    char buffer[BUF_LIMIT];
    int len = readlink(path, buffer, sizeof(buffer) - 1);
    if (len != -1) {
        buffer[len] = '\0';
        strcpy(path, buffer);
    }
}

/*
* This is a very trivial program to copy a file
* It takes two arguments: source-file and target-file
* Source must be a valid file and not a directory
* Target can either be a file or a directory
*/
int main(int argc, char **argv) {

    FILE *file_src, *file_dst;
    char path_src[BUF_LIMIT], path_dst[BUF_LIMIT];
    char buffer[BUF_LIMIT];

    if (argc != 3) {
        handleError("Usage: bbcp src dst\n");
    }

    strcpy(path_src, argv[1]);
    strcpy(path_dst, argv[2]);

    printf("s:%s\nd:%s", path_src, path_dst);
    resolveLink(path_src);
    resolveLink(path_dst);
    printf("\n\ns:%s\nd:%s", path_src, path_dst);

    if (strcmp(path_src, path_dst) == 0) {
        handleError("Source and Destination are same!");
    }

    if (access(path_src, R_OK) != 0) {
        handleError("Access Denied!");
    }

    struct stat buf;
    stat(path_src, &buf);
    if (S_ISREG(buf.st_mode) == 0) {
        handleError("Invalid file");
    }

    file_src = openFile(path_src, "rb");

    if (access((dirname(strdup(path_dst))), W_OK) != 0) {
        handleError("Destination is not writable!");
    }

    /* To create a subdirectory if it does not exist */
    mkdir(dirname(strdup(path_dst)), 0777);

    stat(path_dst, &buf);
    if (S_ISDIR(buf.st_mode)) {
        strcat(path_dst, "/");
        strcat(path_dst, basename(strdup(path_src)));
    }


    file_dst = openFile(path_dst, "wb");


    /* Read from source and write to destination one byte at a time */
    int res;
    while((res = fread(buffer,1,1,file_src)) > 0){
        fwrite(buffer, 1, 1, file_dst);
    }

    /* Close files using our error handling function */

    closeFile(file_src);
    closeFile(file_dst);

    if (res == -1) {
        handleError("Error writing file\n");
    }

    return 0;
}
