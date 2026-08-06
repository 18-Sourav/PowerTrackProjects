#include <stdio.h>
#include <string.h>
#include "view.h"
#include "edit.h"

/* Display the help menu */
static void print_help(void)
{
    printf("\n==================== MP3 TAG READER / EDITOR ====================\n");
    printf("This tool reads and edits the ID3 metadata of an MP3 file.\n\n");

    printf("Menu options:\n");
    printf("  1 - View the tags of the MP3 file\n");
    printf("      Usage: ./mp3_tag_reader -v <filename.mp3>\n");
    printf("  2 - Edit a tag of the MP3 file\n");
    printf("      Usage: ./mp3_tag_reader -e <-t/-a/-A/-y/-g> \"new_value\" <filename.mp3>\n");
    printf("  3 - Display this help menu\n\n");

    printf("Edit options:\n");
    printf("  -t   Edit the Title\n");
    printf("  -a   Edit the Artist\n");
    printf("  -A   Edit the Album\n");
    printf("  -y   Edit the Year (4-digit value)\n");
    printf("  -g   Edit the Genre\n\n");

    printf("Example:\n");
    printf("  ./mp3_tag_reader -e -t \"My Song Title\" Song.mp3\n");
    printf("================================================================\n\n");
}

int main(int argc,char *argv[])
{
    ToviewInfo Toview;
    ToeditInfo Toedit;

    // Check if any command line argument is passed
    if (argc < 2)
    {
        print_help();
        return failure;
    }

    // Determine operation type based on argv[1]
    OperationType op = check_operation_type(argv);

    switch (op)
    {
        case view_mp3:
            if (read_and_validate(argv, &Toview) == failure)
            {
                fprintf(stderr, "Error: Invalid MP3 file.\n");
                fprintf(stderr, "Usage: ./mp3_tag_reader -v <filename.mp3>\n");
                return failure;
            }
            if (to_view(&Toview) == failure)
                return failure;
            break;

        case edit_mp3:
            Toedit.temp_mp3_fname = "temp.mp3";
            if (Read_and_Validation_for_edit(argc, argv, &Toedit) == failure)
                return failure;
            if (to_edit(&Toedit, argv) == failure)
                return failure;
            break;

        case helpmenu:
            print_help();
            break;

        case Invalid_operator:
        default:
            fprintf(stderr, "Error: Invalid option or arguments.\n");
            print_help();
            return failure;
	}

	return 0;
}
