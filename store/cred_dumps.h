#ifndef W0RMNET_CRED_DUMPS_H
#define W0RMNET_CRED_DUMPS_H

typedef struct {
    const char* name;
    const char* shortDescription;
    const char* description;
    const char* contents;
    int numCredentials;
} CredDump;

extern CredDump CredDumpsList[];

extern int CredDumpsListLength;

#endif //W0RMNET_CRED_DUMPS_H
