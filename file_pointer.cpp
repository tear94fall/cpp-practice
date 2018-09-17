#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[]){
    int fileNumber;
    FILE *filePointer;
    fileNumber = open("myfile.txt", O_WRONLY | O_CREAT);
    if(fileNumber<0){
        printf("Can't read file using file\n");
        exit(1);
    }

    write(fileNumber, "Writing usinf File Number\n",17);

    filePointer = fdopen(fileNumber, "a");
    fprintf(filePointer, "Writing using File Pointer\n");
    fclose(filePointer);
    close(fileNumber);

    return 0;
}