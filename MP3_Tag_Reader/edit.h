#ifndef EDIT_H
#define EDIT_H

#include <stdio.h>
#include "types.h"

typedef struct _Toedit
{
    /* mp3 file */
    char *edit_mp3_fname;
    FILE *fptr_edit_mp3_file;
    unsigned int e_size;
    char tag[5];
    int flag;

    /* temp mp3 file */
    char *temp_mp3_fname;
    FILE *fptr_temp_mp3_file;
    char option[5];
    char buffer[100];//to store the content from the user
    uint argv_size; 

} ToeditInfo;




/* read and validation with extension_for_edit*/

Status Read_and_Validation_for_edit(int argc, char *argv[], ToeditInfo *Toedit );

/* Perform to edit*/
Status to_edit(ToeditInfo *Toedit,char *argv[]); // Parent function to be called in main for the edit function

/* Get File pointers for edit */
Status open_files_for_edit(ToeditInfo *Toedit); // open the file that we want to edit

/* check ID3 tag for edit */
Status check_ID3_tag_for_edit(ToeditInfo *Toedit); // check the tag for edit

/* check version for edit */
Status check_version_for_edit(ToeditInfo *Toedit); // check the version of the file present

/* call function for edit */
Status mp3_edit(ToeditInfo *Toedit,char *argv[]); // parent function inside this all the edit functions will be called...before this all the validaiton function are present

/* skip header for edit */
Status skip_header_for_edit(ToeditInfo *Toedit); //copy header to temp

/* read tag for edit */
Status read_tag_for_edit(ToeditInfo *Toedit); // read the tag and compare it to the option

/* read the size */
Status read_size_for_edit(ToeditInfo *Toedit,char *argv[]); // read size from the user

/* read and replace */
Status read_and_replace(ToeditInfo *Toedit,char *argv[]); // when data is matched i.e tag is matched then use this

/* copy data */
Status copy_data(ToeditInfo *Toedit); // when the tag is not matched then copy the normal data to temp

/*copy remain */
Status copy_remaining_data(ToeditInfo *Toedit); // after modification then copy the remaining data from that position copy everything

/* read size for edit without modify */
Status read_size_for_edit_without_modify(ToeditInfo *Toedit); //beofre matching to read the size

/* read and replace without modify */
Status read_and_replace_without_modify(ToeditInfo *Toedit); // before matching of the tags we will call this to copy the content 

/* set flag */
Status set_flag(ToeditInfo *Toedit);

Status copy_temp_to_org(ToeditInfo *Toedit);

int year_validation(char *argv);
	
#endif