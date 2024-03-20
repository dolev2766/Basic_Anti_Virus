/*********************************
* Class: MAGSHIMIM C2             *
* Week:                             *
* Name:                          *
* Credits:                       *
**********************************/

#include <stdio.h>
#include "dirent.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define FIRST_BYTES_QUICK 20
#define END_BYTES_QUICK 80
#define MAX_PRECANTAGE 100
#define FALSE 0
#define TRUE !FALSE

bool findVirus(FILE* virus_sign, FILE* dst, int starting_point, int ending_point);
bool defult(void);
void printMessageError(void);
void freeAll(int max, char* names[], char* file);
void getFiles(char* names[], DIR* checking_dir, char director_path[]);
void sorted(int user_friends_count, char* names[]);
void swap(char** str1, char** str2);
void searchAll(char file_path[], FILE* read_file, FILE* read_virus, FILE* log_file, int first, int end);
void quickScan(char file_path[], FILE* read_file, FILE* read_virus, FILE* log_file);
void welcomeMessage(char sign[], char foleder[], FILE* log_file);
void mangae_options(char* files[], char sign[], int max, FILE* log_file);
int directFilesLength(DIR* direct);
int getFileLen(FILE* file);

/*
    This function is the manage
    input: the cmd parameteg count and the parameter
    output: 0 on sucsses
*/
int main(int argc, char* argv[])
{
    char** names = NULL;
    char* log_path = NULL;
    int files_len = 0;
    DIR* checking_dir = 0;
    FILE* log_file = 0;

    if (argc == 3)
    {
        log_path = (char*)malloc(sizeof(char) * (strlen(argv[1]) + strlen("AntiVirusLog.txt") + 1));
        if (log_path != NULL)
        {
            //creat log file
            strcpy(log_path, argv[1]);
            strcat(log_path, "/");
            strncat(log_path, "AntiVirusLog.txt", strlen("AntiVirusLog.txt"));
            log_file = fopen(log_path, "w+");

            //welcome message
            welcomeMessage(argv[1], argv[2], log_file);

            checking_dir = opendir(argv[1]); //OPEN DIR

            //get directory len
            files_len = directFilesLength(checking_dir);

            //set memory to files list
            names = (char**)malloc(sizeof(char*) * (files_len + 1)); //make memory for files
            closedir(checking_dir); //close dir

            //get files from the dir
            checking_dir = opendir(argv[1]);
            getFiles(names, checking_dir, argv[1]);

            //sort files by name
            sorted(files_len, names);

            //input option and do the scan 
            mangae_options(names, argv[2], files_len, log_file);

            //free names memory
            freeAll(files_len, names, log_path);

            closedir(checking_dir); //close dir
        }
    }
    else
    {
        printMessageError(); //print error
    }


    getchar();
    return 0;
}


/*
    This file will manage the options
    input: the files list, the files len, the sign and the file log
    output: None
*/
void mangae_options(char* files[], char sign[], int max, FILE* log_file)
{
    FILE* read_file = 0;
    FILE* read_virus = 0;
    int i = 0;
    bool user_option = FALSE;

    user_option = defult();
    fprintf(log_file, "%s\n", "Scanning option:");
    user_option ? fprintf(log_file, "%s\n\n", "Quick Scan") : fprintf(log_file, "%s\n\n", "searchAll");

    for (i = 0; i < max; i++)
    {
        read_file = fopen(files[i], "rb");
        if (read_file) //sign isn't null
        {
            read_virus = fopen(sign, "rb");
            if (read_virus) //file isn't null
            {
                user_option ? quickScan(files[i], read_file, read_virus, log_file) : searchAll(files[i], read_file, read_virus, log_file, 0, getFileLen(read_file));
                fclose(read_virus);
            }
            fclose(read_file);
        }
    }
}


/*
    This function do a quick scan to file
    input: file, virus sign, log(to write), the dir path
    output: None
*/
void quickScan(char file_path[], FILE* read_file, FILE* read_virus, FILE* log_file)
{
    bool find = FALSE;
    int file_len = 0;

    file_len = getFileLen(read_file);
    find = findVirus(read_virus, read_file, 0, ((file_len / MAX_PRECANTAGE) * FIRST_BYTES_QUICK)); //check for virus
    find ? (printf("%s - Infected! (first 20%%)\n", file_path), fprintf(log_file, "%s %s\n", file_path, "- Infected! (first 20%)")) : (NULL);
    if (!find)
    {
        find = findVirus(read_virus, read_file, ((file_len / MAX_PRECANTAGE) * END_BYTES_QUICK), file_len); //check all
        find ? (printf("%s - Infected! (last 20%%)\n", file_path), fprintf(log_file, "%s %s\n", file_path, "- Infected! (last 20%)")) : (searchAll(file_path, read_file, read_virus, log_file, ((file_len / MAX_PRECANTAGE) * FIRST_BYTES_QUICK), ((file_len / MAX_PRECANTAGE) * END_BYTES_QUICK)));
    }
}


/*
    This function will check all the data
    input: files array, the virus path and max files
    output: None
*/
void searchAll(char file_path[], FILE* read_file, FILE* read_virus, FILE* log_file, int first, int end)
{
    bool find = FALSE;
    find = findVirus(read_virus, read_file, first, end); //check for virus
    find ? (printf("%s - Infected!\n", file_path), fprintf(log_file, "%s %s\n", file_path, "- Infected!")) : (printf("%s - clean\n", file_path), fprintf(log_file, "%s %s\n", file_path, "- clean"));
}


/*
    This function will say welcome to the user
    and put the print messages in the log folder
    input: sign, folder <str> and log_file to push data in log file<FILE*>
    output: None
*/
void welcomeMessage(char foleder[], char sign[], FILE* log_file)
{
    printf("Welcome to Dolev Virus Scan!\n\n");
    fprintf(log_file, "%s", "Welcome to Dolev Virus Scan!\n\n");
    printf("Folder to scan: %s\n", foleder);
    fprintf(log_file, "%s %s\n", "Folder to scan:", foleder);
    printf("Virus signature: %s\n", sign);
    fprintf(log_file, "%s %s\n\n", "Virus signature:", sign);
}


/*
    This function will get option from the user
    and return the option with true or false
    input: None
    output: the user choice <bool>
*/
bool defult(void)
{
    int user_option = 0;

    //input option
    printf("Press 0 for a normal scan or any other key for a quick scan:");
    scanf("%d", &user_option);
    getchar(); //cleaning the bufffer

    return user_option;
}




/*
    This function is checking if we have virus in file
    input: file, sign, point we want to start checking, point we want to stop checking
    output: if we get virus
*/
bool findVirus(FILE* virus_sign, FILE* file_check, int starting_point, int ending_point)
{
    char ch_virus = 0;
    char ch_file = 0;
    int file_loction = 0;
    int file_loction_loop = 0;

    fseek(file_check, starting_point, SEEK_SET);
    while (!feof(file_check) && !feof(virus_sign) && file_loction != ending_point)
    {
        //set seek of files
        fseek(virus_sign, 0, SEEK_SET);
        fseek(file_check, file_loction, SEEK_SET);
        file_loction = ftell(file_check); //save file loction

        do //checking loop
        {
            ch_virus = (char)fgetc(virus_sign);
            ch_file = (char)fgetc(file_check);
        }         while ((ch_virus == ch_file) && !feof(virus_sign));
        file_loction++;
    }

    return feof(virus_sign);
}


/*
    This function will sorted the name of user friends
    input: the user friends count, list
    output: None
*/
void sorted(int user_friends_count, char* names[])
{
    int i = 0;
    int j = 0;

    for (i = 0; i < user_friends_count; i++)
    {
        for (j = i + 1; j < user_friends_count; j++)
        {
            if (strcmp(names[i], names[j]) > 0)
            {
                swap(&names[i], &names[j]);
            }
        }
    }
}


/*
    This function will swap between two strings
    input: two strings
    output: None
*/
void swap(char** str1, char** str2)
{
    char* temp = *str1;
    *str1 = *str2;
    *str2 = temp;
}


/*
    This function will free the memory
    input: the user friends count, list
    output: None
*/
void freeAll(int max, char* names[], char* file)
{
    int i = 0;
    for (i = 0; i < max; i++)
    {
        free(names[i]);
    }
    free(names);
    free(file);
}


/*
    This function will get all files names
    input: pointer to poiters
    output: None
*/
void getFiles(char* names[], DIR* checking_dir, char director_path[])
{
    struct dirent* file_to_check = 0;
    int i = 0;


    while ((file_to_check = readdir(checking_dir)) != NULL)
    {
        if (strcmp(file_to_check->d_name, ".") && strcmp(file_to_check->d_name, "..") && file_to_check != DT_DIR) //clear the window tabs
        {
            names[i] = (char*)malloc(sizeof(char) * 50); //make memory for word /*************/
            if (names[i] != NULL)
            {
                strcpy(names[i], director_path);
                strcat(names[i], "/");
                strncat(names[i], file_to_check->d_name, (int)strlen(file_to_check->d_name) + 1);
                i++;
            }
        }
    }
}


/*
    This function will print the error message
    input: None
    output: None
*/
void printMessageError(void)
{
    printf("Error: the protocol - file.exe <folder path> <virus sign path>");
}



/*
    This function will get file and return his len
    input: file <type: FILE*>
    output: length <type: int>
*/
int getFileLen(FILE* file)
{
    int length = 0;

    fseek(file, 0, SEEK_END);
    length = ftell(file);

    fseek(file, 0, SEEK_SET); //reset position
    return length;
}


/*
    This function is for get the length of directory
    input: directory
    output: the length
*/
int directFilesLength(DIR* direct)
{
    int count = 0;
    struct dirent* file = 0;

    while ((file = readdir(direct)) != NULL)
    {
        if (strcmp(file->d_name, ".") && strcmp(file->d_name, "..") && file->d_name != DT_DIR) //clear the window tabs
        {
            count++;
        }
    }

    return count;
}
