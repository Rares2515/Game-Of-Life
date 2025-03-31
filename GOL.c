#include <stdio.h>
#include <stdlib.h>
#include <string.h>




int main(int argc, const char *argv[])
{

	if (argc < 3) {
        printf("Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1; 
    }

    FILE *input_file = fopen(argv[1], "r");
    FILE *output_file = fopen(argv[2], "w");


    if (input_file == NULL || output_file == NULL) {
        printf("Could not open files.\n");
        if (input_file != NULL) fclose(input_file);
        if (output_file != NULL) fclose(output_file);
        return 1;
    }



	fclose(input_file);
	fclose(output_file);



return 0;
}