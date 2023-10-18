#include <stdio.h>
#include <string.h>
#include <glib.h>
#include "components.h"

char* comp_position_serialize(Position* position) {
    char buffer[100];

    sprintf(buffer, "%f;%f", position->coord.x, position->coord.y);

    char* data = calloc(strlen(buffer)+1, sizeof(char));
    strcpy(data, buffer);

    return data;
}

Position* comp_position_deserialize(char* data) {
    Position* position = calloc(1, sizeof(Position));

    char** splitData = g_strsplit(data, ";", 2);

    sscanf(splitData[0], "%f", &position->coord.x);
    sscanf(splitData[1], "%f", &position->coord.y);

    g_strfreev(splitData);

    return position;
}
