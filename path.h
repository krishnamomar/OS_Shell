#define MAX_INP_SIZE 400

#ifndef __WD_STRUCT__
#define __WD_STRUCT__

typedef struct WD{
    char previous[MAX_INP_SIZE];
    char start[MAX_INP_SIZE];
    char current[MAX_INP_SIZE];
} WD;

extern WD working_path;

#endif
