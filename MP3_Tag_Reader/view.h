#ifndef ENCODE_H
#define ENCODE_H

#include <stdio.h>
#include "types.h" // Contains user defined types

typedef struct _Toview
{
    /* mp3 file */
    char *mp3_fname;
    FILE *fptr_mp3_file;
    FILE *fptr_temp_file;
    unsigned int size;
    uint loop;
    char frame_id[5];
    unsigned int frame_size;
} ToviewInfo;


/* Check operation type */
OperationType check_operation_type(char *argv[]);

/* read and validation with extension */
Status read_and_validate(char *argv[], ToviewInfo *Toview );

/* Perform to view */
Status to_view(ToviewInfo *Toview); // parent function to be called in the main for view function

/* Get File pointers */
Status open_files(ToviewInfo *Toview);// open files for edit ....open both file...song in read mode and temp in write mode

/* check ID3 tag */
Status check_ID3_tag(ToviewInfo *Toview);// cheak for ide3 tag version 2 or 3

/* check version  */
Status check_version(ToviewInfo *Toview); 

/* call function for view */
Status mp3_view(ToviewInfo *Toview);

/* skip header */
Status skip_header(ToviewInfo *Toview);

/* read the tag */
Status read_size(ToviewInfo *Toview);

/* read the contents */
Status Read_contents(ToviewInfo *Toview);


#endif
