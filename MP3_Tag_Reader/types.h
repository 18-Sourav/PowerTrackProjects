#ifndef TYPES_H
#define TYPES_H

/* User defined types */
typedef unsigned int uint;

/* Status will be used in fn. return type */
typedef enum
{
    success,
    failure
} Status;

typedef enum
{
    view_mp3,
    edit_mp3,
    helpmenu,
    Invalid_operator
} OperationType;

#endif