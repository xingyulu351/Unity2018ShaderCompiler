#pragma once

#include <stdio.h>

#define TCDie(error, ...) {\
    fprintf (stderr, "CRITICAL: %s:%d " error, __FILE__, __LINE__, ##__VA_ARGS__);\
    exit (-1);\
}

#define TCError(error, ...) {\
    fprintf (stderr, "ERROR: %s:%d " error, __FILE__, __LINE__, ##__VA_ARGS__);\
}


#define TCWarn(error, ...) {\
    fprintf (stderr, "WARNING: %s:%d " error, __FILE__, __LINE__, ##__VA_ARGS__);\
}

#define TCMessage(msg, ...) {\
    fprintf (stderr, msg, ##__VA_ARGS__);\
}
