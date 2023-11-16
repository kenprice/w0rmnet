#ifndef W0RMNET_FILES_H
#define W0RMNET_FILES_H

#include "exploits.h"

#define FILE_NAME_MAX_LEN 50

typedef enum {
    FileTypeCredentials,
    FileTypeExploit,
} FileType;

typedef union {
    Exploit* exploit;
} FileContent;

typedef struct {
    char name[FILE_NAME_MAX_LEN];
    FileContent content;
} File;

typedef struct Directory {
    char name[FILE_NAME_MAX_LEN];
    struct Directory** directories;
    int numDirectories;
    struct File** files;
    int numFiles;
} Directory;

#endif //W0RMNET_FILES_H
