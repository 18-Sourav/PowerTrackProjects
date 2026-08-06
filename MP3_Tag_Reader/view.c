#include "view.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


//Function to validate the input
OperationType check_operation_type(char *argv[])  //checking for the operation type provided in the CLA
{
    if(argv[1]==NULL)
    {
        fprintf(stderr, "Error: Invalid Input. Please enter the operation in the CLA.\n");
        return Invalid_operator;
    }
    else if(strcmp(argv[1],"-v")==0)
        return view_mp3;
    
    else if(strcmp(argv[1],"-e")==0)
        return edit_mp3;
    
    else if(strcmp(argv[1] ,"--help")==0 || strcmp(argv[1],"-h")==0)
        return helpmenu;
    
    else
        return Invalid_operator;
    
}


Status read_and_validate(char *argv[], ToviewInfo *Toview)
{
    if (argv[2] == NULL)
        return failure;

    char *ext = strrchr(argv[2], '.');
    if (ext == NULL || strcmp(ext, ".mp3") != 0)
        return failure;

    Toview->mp3_fname = argv[2];
    return success;
}

// Function to open both the files 
Status open_files(ToviewInfo *Toview)
{
    Toview->fptr_mp3_file=fopen(Toview->mp3_fname,"r");

    if(Toview->fptr_mp3_file==NULL)
    {
        perror("fopen");
        return failure;
    }

    Toview->fptr_temp_file=fopen("temp.mp3","w");
    if(Toview->fptr_temp_file==NULL)
    {
        perror("fopen");
        return failure;
    }
    return success;
}
Status check_ID3_tag(ToviewInfo *Toview)
{
    char str[4] = {0};
    if(fread(str,1,3,Toview->fptr_mp3_file)!=3)  // Reading the raw data of the ID
        return failure;
    if(strcmp(str,"ID3")==0)
    {
        printf("ID3 version is found\n");
        return success;
    }
    return failure;
}

Status check_version(ToviewInfo *Toview)  //reading and checking the version of the ID
{
    char str[2];
    if(fread(str,sizeof(char),2,Toview->fptr_mp3_file)!=2)  // Reading the raw data of the version
        return failure;
    
    if(str[0] ==3 || str[0]== 4)
    {
        printf("Version ID3v2.%d.%d is found\n", str[0], str[1]);
        return success;
    }
    return failure;   
}

/* skip header */
Status skip_header(ToviewInfo *Toview)
{
    if(fseek(Toview->fptr_mp3_file,1,SEEK_CUR)!=0)
        return failure;
    return success;    
}

//Reading the file size in total
Status read_size(ToviewInfo *Toview)
{
    unsigned char size[4];

    if(fread(size,sizeof(char),4,Toview->fptr_mp3_file)!=4)   //The size stored will be in BIG Endian
        return failure;

    //Now need to convert to little endian and skipping the 7th bit in each byte
    unsigned int n_size;

    n_size = (size[0] & 0x7F)<<21;    // taking 7 bits and shifting 21 places as total we have 7*4 = 28 bytes
    n_size |= (size[1] & 0x7F)<<14;
    n_size |= (size[2] & 0x7F)<<7;
    n_size |= (size[3] & 0x7F);

    Toview->size = n_size;         // saving this to the size member of the structure

    return success;
}

Status to_view(ToviewInfo *Toview)
{
    if (open_files(Toview) == failure)
        return failure;
    if (check_ID3_tag(Toview) == failure)
        return failure;
    if (check_version(Toview) == failure)
        return failure;
    if (skip_header(Toview) == failure)
        return failure;
    if (read_size(Toview) == failure)
        return failure;
    if (mp3_view(Toview) == failure)
        return failure;        
    return success;
}


 //Function to iterates through all the frames inside the ID3v2 tag of the MP3 file
 //and prints the metadata (title, artist, album, etc.) of each frame.
Status mp3_view(ToviewInfo *Toview)
{
    // Calculate the byte offset where the tag data ends,
    // i.e. current position (after the tag header) + tag size.
    long tag_end = ftell(Toview->fptr_mp3_file) + Toview->size;

    // Keep reading frames until we reach the end of the tag.
    while (ftell(Toview->fptr_mp3_file) < tag_end)
    {
        // Read the 4-byte frame ID which tells us what the frame contains.
        if (fread(Toview->frame_id, 1, 4, Toview->fptr_mp3_file) != 4)
            return failure;

        // Null-terminate the frame ID so it can be used as a string.
        Toview->frame_id[4] = '\0';

        // A zero byte in the frame ID marks the start of padding,
        // so stop scanning further frames.
        if (Toview->frame_id[0] == '\0')
            break;

        // Read the 4-byte frame size (big-endian format).
        unsigned char size_buf[4];
        if (fread(size_buf, 1, 4, Toview->fptr_mp3_file) != 4)
            return failure;

        // Combine the 4 bytes into a single unsigned integer (big-endian).
        Toview->frame_size = (size_buf[0] << 24) | (size_buf[1] << 16) | (size_buf[2] << 8) | size_buf[3];

        // A frame size of zero means there is no data, stop scanning.
        if (Toview->frame_size == 0)
            break;

        // Skip the 2-byte frame flags (not needed for viewing).
        if (fseek(Toview->fptr_mp3_file, 2, SEEK_CUR) != 0)
            return failure;

        // Read and print the actual content of this frame.
        if (Read_contents(Toview) == failure)
        return failure;
    }
    return success;
}

Status Read_contents(ToviewInfo *Toview)
{
    // Read the first byte of the frame data which specifies the text encoding.
    unsigned char encoding;
    if (fread(&encoding, 1, 1, Toview->fptr_mp3_file) != 1)
        return failure;

    // The remaining bytes are the actual content
    // (total frame size minus the 1 encoding byte).
    unsigned int data_size = Toview->frame_size - 1;

    // Allocate memory for the content (+1 for the null terminator).
    char *data = malloc(data_size + 1);
    if (data == NULL)
        return failure;

    // Read the actual text content of the frame.
    if (fread(data, 1, data_size, Toview->fptr_mp3_file) != data_size)
    {
        free(data);
        return failure;
    }

    // Null-terminate the string so it can be printed safely.
    data[data_size] = '\0';

    // Match the frame ID against known tags and print a friendly label.
    if (strcmp(Toview->frame_id, "TIT2") == 0)
        printf("Title       : %s\n", data);
    else if (strcmp(Toview->frame_id, "TPE1") == 0)
        printf("Artist      : %s\n", data);
    else if (strcmp(Toview->frame_id, "TALB") == 0)
        printf("Album       : %s\n", data);
    else if (strcmp(Toview->frame_id, "TYER") == 0)
        printf("Year        : %s\n", data);
    else if (strcmp(Toview->frame_id, "TCON") == 0)
        printf("Genre       : %s\n", data);
    else if (strcmp(Toview->frame_id, "TRCK") == 0)
        printf("Track       : %s\n", data);
    else if (strcmp(Toview->frame_id, "TENC") == 0)
        printf("Encoded By  : %s\n", data);
    else if (strcmp(Toview->frame_id, "COMM") == 0)
        printf("Comment     : %s\n", data);
    else
        // For any unknown frame, print the raw frame ID and its content.
        printf("%-12s: %s\n", Toview->frame_id, data);

    free(data);
    return success;
}