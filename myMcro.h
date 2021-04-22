#pragma once
#ifndef __MYMACROS__
#define __MYMACROS__

#include <stdio.h>

#define CHECK_RETURN_0(p) if (!p) return 0;
#define CHECK_RETURN_NULL(p) if (!p) return NULL;

#define CHECK_MSG_RETURN_0(p, s) \
{ \
    if (!p) \
    { \
        printf("%s\n", s); \
        return 0; \
    } \
}

#define CHECK_0_MSG_CLOSE_FILE(v, f, s) \
{ \
    if (!v) \
    { \
        printf("%s\n", s); \
        fclose(f); \
        return 0; \
    } \
}

#define CHECK_NULL_MSG_CLOSE_FILE(v, f, s) \
{ \
    if (!v) \
    { \
        printf("%s\n", s); \
        fclose(f); \
        return 0; \
    } \
}

#define MSG_CLOSE_RETURN_0(f, s) \
{ \
    printf("%s\n", s); \
    fclose(f); \
    return 0; \
}

#endif