/*
 * Title of program
 * Author Name
 * Other info
*/

#include <sys/stat.h>
#include <fcntl.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/unistd.h>
#include <unistd.h>

/*
 * This function prints an error message to stderr and exits with a positive
 * exit code
 */
void handleError(const char* msg){
    fprintf(stderr, "%s", msg);
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

/*
 * Check if its a relative path and appends './' at the start if necessary
 */
void checkRelativePath(char* path){

    char tmp[1024];
    unsigned int len = strlen(path);

    if(len == 0)
        return;

    if(path[0] == '/')
        return;

    if(len >= 2 && path[0] == '.' && path[1] == '/')
        return;

    if(len >= 3 && path[0] == '.' && path[1] == '.' && path[2] == '/')
        return;


    strcpy(tmp, "./");
    strcat(tmp, path);
    strcpy(path, tmp);
}

/*
 * The program takes a source file and copies it to a target file,
 * the source must be a file and the target can be a file or a
 * containing directory
 */
int main(int argc, char* argv[]) {

    if(argc < 2){
        handleError("Invalid arguments, please specify a source and destination!");
    }

    if (strcmp(argv[1], argv[2]) == 0) {
        handleError("Source and destination same!");
    }

    if (access(argv[1], R_OK) != 0) {
        handleError("File is not readable (access denied)\n");
        exit(1);
    }

    char src[1024];
    char dst[1024];

    /* Setting source address */

    strcpy(src, argv[1]);
    strcpy(dst, argv[2]);
    checkRelativePath(src);
    checkRelativePath(dst);



    /* Setting destination address */

    struct stat s;
    if(stat(dst, &s) == 0){
        if(s.st_mode & S_IFDIR){
            strcpy(dst, dst);
            strcat(dst, basename(src));
        }
    }

    /* Open files using our error handling function */

    FILE * inFile = openFile(src, "rb");
    FILE * outFile = openFile(dst, "wb");


    /* Read from source and write to destination one byte at a time */
    char buf[512];
    while(fread(buf,1,1,inFile)){
        fwrite(buf, 1, 1, outFile);
    }

    /* Close files using our error handling function */

    closeFile(inFile);
    closeFile(outFile);

    return 0;
}
