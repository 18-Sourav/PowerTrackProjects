#include "edit.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//Function to check the CLA input
Status Read_and_Validation_for_edit(int argc, char *argv[], ToeditInfo *Toedit )
{

    if (argc < 5)
    {
        fprintf(stderr,"Error: Insufficient arguments.\n");
        fprintf(stderr,"Usage: ./a.out -e <-t/-a/-A/-y/-g> \"new_value\" filename.mp3\n");
        return failure;
    }

    //Checking the song file extension to be edited
    char *extn = NULL;
    extn = strrchr(argv[4],'.');
    if (extn == NULL || strcmp(extn, ".mp3") != 0)
    {
        fprintf(stderr,"Error: File is not a valid .mp3 file.\n");
        return failure;
    }

    //storing the song filne name
    Toedit->edit_mp3_fname = argv[4];
    
    //checking the tag and storing it
    if (strcmp(argv[2], "-t") == 0 || 
    strcmp(argv[2], "-a") == 0 || 
    strcmp(argv[2], "-A") == 0 || 
    strcmp(argv[2], "-y") == 0 || 
    strcmp(argv[2], "-g") == 0)
    {
        // Valid tag option found!
        strcpy(Toedit->option, argv[2]);
    }
    else
    {
        fprintf(stderr,"Error: Invalid option. Use -t, -a, -A, -y, or -g.\n");
        return failure;
    }

    return success;
}
/* Perform to edit*/
Status to_edit(ToeditInfo *Toedit,char *argv[]) //Parent function to be called in the main for edit function
{
    //Open the mp3 file in read mode and the temp file in write mode
    if (open_files_for_edit(Toedit) == failure)
        return failure;

    //Check whether the file starts with the ID3 tag
    if (check_ID3_tag_for_edit(Toedit) == failure)
        return failure;

    //Check whether the ID3 version is supported (v2.3 / v2.4)
    if (check_version_for_edit(Toedit) == failure)
        return failure;

    //Locate the requested frame and write the modified copy to the temp file
    if (mp3_edit(Toedit, argv) == failure)
        return failure;

    //If the tag was never found, keep the original file untouched
    if (Toedit->flag == 0)
    {
        fprintf(stderr, "Error: The requested tag was not found.\n");
        return failure;
    }

    //Copy the modified data from the temp file back to the original file
    if (copy_temp_to_org(Toedit) == failure)
        return failure;

    return success;
}

/* Get File pointers for edit */
Status open_files_for_edit(ToeditInfo *Toedit)
{
    //Open the mp3 file to be edited in read mode
    Toedit->fptr_edit_mp3_file = fopen(Toedit->edit_mp3_fname,"r");
    if(Toedit->fptr_edit_mp3_file==NULL)
    {
        fprintf(stderr,"The song was unable to open\n");
        return failure;
    }

    //Give the temp file a default name if none was provided
    if (Toedit->temp_mp3_fname == NULL)
        Toedit->temp_mp3_fname = "temp.mp3";

    //Open the temp file in read/write mode to store the edited copy of the song
    Toedit->fptr_temp_mp3_file = fopen(Toedit->temp_mp3_fname,"w+b");
    if(Toedit->fptr_temp_mp3_file==NULL)
    {
        fprintf(stderr,"The temp file was unable to open\n");
        return failure;
    }

    return success;
}

/* check ID3 tag for edit */
Status check_ID3_tag_for_edit(ToeditInfo *Toedit)
{
    char id[4]= {0};
    if(fread(id,sizeof(char),3,Toedit->fptr_edit_mp3_file)!=3)
    {
        fprintf(stderr,"Reading the ID failed\n");
        return failure;
    }
    id[3] = '\0';
    if (strcmp(id, "ID3") == 0)  //checking the version 3 of the file
    {
        printf("INFO: ID3 tag found successfully.\n");
        return success;
    }

    fprintf(stderr, "Error: Not a valid ID3 tag.\n");
    return failure;
}

/* check version for edit */
Status check_version_for_edit(ToeditInfo *Toedit) // check the version of the file present
{
    char version[2] = {0};

    if(fread(version,sizeof(char),2,Toedit->fptr_edit_mp3_file)!=2)
    {
        fprintf(stderr,"Reading the version failed\n");
        return failure;
    }
    //Checking the version for to be 3 or 4 
    if(version[0]==3 || version[0]==4)
    {
        printf("Version 2.%d found sucessfully\n",version[0]);
        return success;
    }
    fprintf(stderr, "Error: Unsupported ID3 version.\n");
    return failure;
}

/* call function for edit */
Status mp3_edit(ToeditInfo *Toedit,char *argv[])
{
    //Validate the year value when the -y option is used
    if (strcmp(Toedit->option, "-y") == 0)
    {
        if (year_validation(argv[3]) == failure)
        {
            fprintf(stderr, "Error: Invalid year. Use a 4-digit year.\n");
            return failure;
        }
    }

    //No tag has been replaced yet
    Toedit->flag = 0;

    //Map the option given by the user to the actual frame id
    char frame_id[5] = {0};
    if (strcmp(Toedit->option, "-t") == 0)
        strcpy(frame_id, "TIT2");
    else if (strcmp(Toedit->option, "-a") == 0)
        strcpy(frame_id, "TPE1");
    else if (strcmp(Toedit->option, "-A") == 0)
        strcpy(frame_id, "TALB");
    else if (strcmp(Toedit->option, "-y") == 0)
        strcpy(frame_id, "TYER");
    else if (strcmp(Toedit->option, "-g") == 0)
        strcpy(frame_id, "TCON");
    else
    {
        fprintf(stderr, "Error: Invalid option.\n");
        return failure;
    }

    //Copy the 10-byte ID3 header to the temp file
    if (skip_header_for_edit(Toedit) == failure)
        return failure;

    //Walk through all the frames one by one
    while (read_tag_for_edit(Toedit) == success)
    {
        //A zero byte in the frame id marks the start of the padding,
        //so stop scanning further frames
        if (Toedit->tag[0] == '\0')
            break;

        //If this frame is the one the user wants to edit, replace its value
        if (strcmp(Toedit->tag, frame_id) == 0)
        {
            //Read the size of the new value supplied by the user
            if (read_size_for_edit(Toedit, argv) == failure)
                return failure;

            //Write the new frame (tag + new value) to the temp file
            if (read_and_replace(Toedit, argv) == failure)
                return failure;

            //Remember that the tag was found and edited
            if (set_flag(Toedit) == failure)
                return failure;

            break;
        }
        else
        {
            //Tag does not match, so copy this whole frame to the temp file
            if (copy_data(Toedit) == failure)
                return failure;
        }
    }

    //After the edited (or the last) frame, copy everything that remains
    if (copy_remaining_data(Toedit) == failure)
        return failure;

    return success;
}

/* skip header for edit */
Status skip_header_for_edit(ToeditInfo *Toedit) //copy header to temp
{
    char header[10];

    //The header is the first 10 bytes of the file, so go back to the start
    rewind(Toedit->fptr_edit_mp3_file);
    if(fread(header,sizeof(char),10,Toedit->fptr_edit_mp3_file)!=10)
    {
        fprintf(stderr,"Skipping of the header failed\n");
        return failure;
    }

    //Write the raw header bytes to the temp file
    if(fwrite(header,sizeof(char),10,Toedit->fptr_temp_mp3_file)!=10)
    {
        fprintf(stderr,"Writing of the header failed\n");
        return failure;
    }
    return success;
}

/* read tag for edit */
Status read_tag_for_edit(ToeditInfo *Toedit) // read the tag and compare it to the option
{
    char tag[5]={0};

    if(fread(tag,sizeof(char),4,Toedit->fptr_edit_mp3_file)!=4)
    {
        fprintf(stderr,"Reading the tag failed\n");
        return failure;
    }
    tag[4] = '\0';
    strcpy(Toedit->tag,tag);

    return success;
}

/* read the size */
Status read_size_for_edit(ToeditInfo *Toedit,char *argv[]) // read size from the user
{
    //The new frame size is the length of the user value plus
    //one extra byte for the text encoding byte
    Toedit->e_size = strlen(argv[3]) + 1;
    return success;
}

/* read and replace */
Status read_and_replace(ToeditInfo *Toedit,char *argv[]) // when data is matched i.e tag is matched then use this
{
    unsigned char old_size[4];
    unsigned char new_size[4];
    char flags[2];
    char encoding = 0;

    //Read the 4-byte size of the old frame (big-endian format)
    if(fread(old_size,sizeof(char),4,Toedit->fptr_edit_mp3_file)!=4)
        return failure;

    //Read the 2 frame flags so that they can be preserved
    if(fread(flags,sizeof(char),2,Toedit->fptr_edit_mp3_file)!=2)
        return failure;

    //Convert the new frame size into a big-endian byte array
    new_size[0] = (Toedit->e_size >> 24) & 0xFF;
    new_size[1] = (Toedit->e_size >> 16) & 0xFF;
    new_size[2] = (Toedit->e_size >> 8) & 0xFF;
    new_size[3] = Toedit->e_size & 0xFF;

    //Write the new frame to the temp file: id + size + flags + encoding + text
    if(fwrite(Toedit->tag,1,4,Toedit->fptr_temp_mp3_file)!=4)
        return failure;
    if(fwrite(new_size,1,4,Toedit->fptr_temp_mp3_file)!=4)
        return failure;
    if(fwrite(flags,1,2,Toedit->fptr_temp_mp3_file)!=2)
        return failure;
    if(fwrite(&encoding,1,1,Toedit->fptr_temp_mp3_file)!=1)
        return failure;
    if(fwrite(argv[3],1,strlen(argv[3]),Toedit->fptr_temp_mp3_file)!=strlen(argv[3]))
        return failure;

    //Skip the old frame data in the source file
    unsigned int old_frame_size = (old_size[0] << 24) | (old_size[1] << 16) | (old_size[2] << 8) | old_size[3];
    if(fseek(Toedit->fptr_edit_mp3_file, old_frame_size, SEEK_CUR)!=0)
        return failure;

    return success;
}

/* copy data */
Status copy_data(ToeditInfo *Toedit) // when the tag is not matched then copy the normal data to temp
{
    //Read the size of the unmatched frame
    if(read_size_for_edit_without_modify(Toedit) == failure)
        return failure;

    //Write the frame id of the unmatched frame to the temp file
    if(fwrite(Toedit->tag,1,4,Toedit->fptr_temp_mp3_file)!=4)
        return failure;

    //Write the 4-byte frame size (big-endian) of the unmatched frame to the temp file
    unsigned char size[4];
    size[0] = (Toedit->e_size >> 24) & 0xFF;
    size[1] = (Toedit->e_size >> 16) & 0xFF;
    size[2] = (Toedit->e_size >> 8) & 0xFF;
    size[3] = Toedit->e_size & 0xFF;
    if(fwrite(size,1,4,Toedit->fptr_temp_mp3_file)!=4)
        return failure;

    //Copy the whole frame (flags + data) to the temp file unchanged
    if(read_and_replace_without_modify(Toedit) == failure)
        return failure;

    return success;
}

/*copy remain */
Status copy_remaining_data(ToeditInfo *Toedit) // after modification then copy the remaining data from that position copy everything
{
    char ch;

    //Copy everything from the current position to the end of the file
    while(fread(&ch,1,1,Toedit->fptr_edit_mp3_file)==1)
    {
        if(fwrite(&ch,1,1,Toedit->fptr_temp_mp3_file)!=1)
            return failure;
    }

    return success;
}

/* read size for edit without modify */
Status read_size_for_edit_without_modify(ToeditInfo *Toedit) //beofre matching to read the size
{
    unsigned char size[4];

    //Read the 4-byte frame size (big-endian format)
    if(fread(size,1,4,Toedit->fptr_edit_mp3_file)!=4)
        return failure;

    Toedit->e_size = (size[0] << 24) | (size[1] << 16) | (size[2] << 8) | size[3];

    return success;
}

/* read and replace without modify */
Status read_and_replace_without_modify(ToeditInfo *Toedit) // before matching of the tags we will call this to copy the content
{
    char flags[2];
    char *data;

    //Read and copy the 2 frame flags
    if(fread(flags,1,2,Toedit->fptr_edit_mp3_file)!=2)
        return failure;
    if(fwrite(flags,1,2,Toedit->fptr_temp_mp3_file)!=2)
        return failure;

    //Allocate memory for the frame data
    data = malloc(Toedit->e_size);
    if(data == NULL)
        return failure;

    //Read and copy the frame data
    if(fread(data,1,Toedit->e_size,Toedit->fptr_edit_mp3_file)!=Toedit->e_size)
    {
        free(data);
        return failure;
    }
    if(fwrite(data,1,Toedit->e_size,Toedit->fptr_temp_mp3_file)!=Toedit->e_size)
    {
        free(data);
        return failure;
    }

    free(data);
    return success;
}

/* set flag */
Status set_flag(ToeditInfo *Toedit) //mark that the tag was found and edited
{
    Toedit->flag = 1;
    return success;
}

/* copy temp to org */
Status copy_temp_to_org(ToeditInfo *Toedit) //copy the modified data back to the original file
{
    FILE *fptr_org;
    char ch;

    //The read stream of the original file is no longer needed before rewriting it
    fclose(Toedit->fptr_edit_mp3_file);

    //Go to the beginning of the temp file
    rewind(Toedit->fptr_temp_mp3_file);

    //Reopen the original mp3 in write mode (this truncates the file)
    fptr_org = fopen(Toedit->edit_mp3_fname, "wb");
    if(fptr_org == NULL)
    {
        fprintf(stderr, "Error: Unable to open the mp3 file for writing.\n");
        return failure;
    }

    //Copy the modified data from the temp file back to the original file
    while(fread(&ch,1,1,Toedit->fptr_temp_mp3_file)==1)
    {
        if(fwrite(&ch,1,1,fptr_org)!=1)
        {
            fclose(fptr_org);
            return failure;
        }
    }

    fclose(fptr_org);
    fclose(Toedit->fptr_temp_mp3_file);

    //Remove the temporary file
    remove(Toedit->temp_mp3_fname);

    printf("INFO: Tag edited successfully.\n");
    return success;
}

/* validate the year */
int year_validation(char *argv) //check that the year is a 4-digit number
{
    //The year must be exactly 4 digits long
    if(strlen(argv) != 4)
        return failure;

    //Every character of the year must be a digit
    for(int i = 0; i < 4; i++)
    {
        if(argv[i] < '0' || argv[i] > '9')
            return failure;
    }

    return success;
}
