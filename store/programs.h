#ifndef W0RMNET_PROGRAMS_H
#define W0RMNET_PROGRAMS_H

typedef enum {
    PROCESS_TYPE_PING,
    PROCESS_TYPE_ECHO,
    PROCESS_TYPE_SCAN,
    PROCESS_TYPE_LOGIN,
    PROCESS_TYPE_CHECK_ORIGIN,
} ProgramType;

typedef struct {
    ProgramType type;
    const char* name;
    int version;
    int versionMinor;
} Program;

extern const char* ProgramTypeLabel[];

extern const char* ProgramNames[][5];

extern Program PingPrograms[];
extern Program EchoPrograms[];
extern Program ScanPrograms[];
extern Program LoginPrograms[];
extern Program CheckOriginPrograms[];

#endif //W0RMNET_PROGRAMS_H
