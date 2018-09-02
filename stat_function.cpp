#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#pragma push_macro("minor")
#undef minor

int main(int argc, char *argv[]){
    time_t t;
    struct stat statbuf;

    if(stat("./stat_test.cpp", &statbuf) == -1){
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    // major/minor number print
    printf("major = %1d \n", major(statbuf.st_dev));
    printf("minor = %1d \n", statbuf.st_dev);

    // file total size printf
    printf("file size = %1d \n", statbuf.st_size);

    // file modify time print
    t = statbuf.st_mtime;
    printf("%s\n", ctime(&t));

    // file mode check, the macro is defined to <sys/stat.h>
    // if((statbuf.st_mode & S_IFMT) == S_IFREG)
    if(S_ISREG(statbuf.st_mode)){
        printf("regular file\n");
    }

    exit(EXIT_SUCCESS);
}

#pragma pop_macro("minor")