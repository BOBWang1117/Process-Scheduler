#include "iotxt.h"

/*
*     Effect: pass path parameters
*Description: Obtain the absolute path str of the user's txt file,
*             and transfer str to the private type strPass
*/
IoTxt::IoTxt(char* str)
{
    strPass = str;
}
/*
*     Effect: count (number of lines in txt file)
*Description: Open the txt file, when you read it
*             and you encounter a newline character, change the line value,
*             and finally get the total number of lines in the txt file
*/
int IoTxt::txtLine()
{
    FILE* fd;
    line = 0;
    if (fd = fopen(strPass, "r"))
    {
        while (!feof(fd))
        {
            if ('\n' == fgetc(fd))
            {
                line++;
            }
        }
    }
    if (fd)
    {
        fclose(fd);
    }

    return line;
}

/*
*     Effect: input file
*Description: The function of inputting the file from
*             the computer into the program
*/
void IoTxt::ioOut()
{
    //Firstly, we need to read a file
    FILE* fp;
    char str[255];
    int row = 0;
    /* This is used to open the file which can read */
    fp = fopen(strPass, "r");
    if (fp == NULL)
    {
        perror("ERROR OCCUR: When you open the file!\n");
        exit(0);
    }
    store = (char**)malloc(sizeof(char*) * line);
    while (fgets(str, 255, fp) != NULL)
    {
        /* Write content to standard output (stdout) */
        store[row] = (char*)malloc(sizeof(char) * strlen(str));
        strcpy(store[row], str);
        row++;
    }
    fclose(fp);
}

/*
*     Effect: pass the array
*Description: Provides the transfer of cross-cpp files
*/
char** IoTxt::stringArrayCopy()
{
    return store;
}