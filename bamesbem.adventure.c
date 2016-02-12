//
//  main.c
//  Project2_OperatingSystems
//
//  Created by Michael Bamesberger on 2/3/16.
//  Copyright (c) 2016 Michael Bamesberger. All rights reserved.
//

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>

#define _GNU_SOURCE

int generateFiles(char name[], char path[]);
void createFiles(char roomNameArray[10][15], char dirPath[20]);
void fillFiles(char arr[7][15], char path[]);
char* makeConnection(char arr[7][15], char filename[15]);
void navigate(char path[]);
int printFile(int i, char fileArray[7][15], char path[]);


int main(void) {
    
    // Create directory name
    char dirPath[21];
    long processId = getpid();
    
    sprintf(dirPath, "%s%ld", "bamesbem.rooms.", processId);

    const int dir_err = mkdir( dirPath , S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (-1 == dir_err)
    {
        printf("Error creating directory!n");
        exit(1);
    }

    char roomNameArray[10][15];
    strcpy(roomNameArray[0], "Pantry");
    strcpy(roomNameArray[1], "Closet");
    strcpy(roomNameArray[2], "Bathroom");
    strcpy(roomNameArray[3], "Kitchen");
    strcpy(roomNameArray[4], "Garage");
    strcpy(roomNameArray[5], "Library");
    strcpy(roomNameArray[6], "Basement");
    strcpy(roomNameArray[7], "Bedroom");
    strcpy(roomNameArray[8], "Office");
    strcpy(roomNameArray[9], "Parlour");
    
    strcat(dirPath, "/");
    // Create files, fill them with connections, begin game
    // The program will return once the final room is found
    
    createFiles(roomNameArray, dirPath);
  
    printf("\nThanks for playing!\n");
    

}

void createFiles(char roomNameArray[10][15], char dirPath[21]){
    srand(time(NULL));
    
    char roomsAvailable[7][15];
    
    //Generate seven files
    int i;
    for (i = 0; i < 7; i++){
        
        
        int r = ( rand() % 9 ) + 1;
        
        char nameFile[25];
        int count = 0;
        int d;
        for (d = 0; d < 15; d++){
            int c = roomNameArray[r][d];
            if((c>='a' && c<='z') || (c>='A' && c<='Z') || (c == ' ') || (c == '\0')){
                nameFile[d] = roomNameArray[r][d];
                count = count + 1;
            }
        }
        
        char *filepath = malloc(strlen(dirPath) + strlen(nameFile));
        filepath = strcpy(filepath, dirPath );
        filepath = strcat(filepath, nameFile);
        
        // If the file already exists, try again
        if( access( filepath, F_OK ) != -1 ) {

            i--;
        }
        else{
            strcpy(roomsAvailable[i], nameFile);
            generateFiles(nameFile, filepath);
        }
        
        int x;
        for (x = 0; x < count; x++){
            nameFile[x] = 0;
            
        }
        free(filepath);
    
    }

    fillFiles(roomsAvailable, dirPath);
    
}

int generateFiles(char name[], char path[]){
    
    FILE *fp;
    
    fp = fopen(path, "w+");
    if (!fp){
        printf("Error. File could not be created\n");
        return 1;
    }
    fclose(fp);
    return 0;
    
}

void fillFiles(char arr[7][15], char path[]){
    
    int roomCount = 0;
    char nameFile[15];
    int i;
    for (i = 0; i < 7; i++){            // address each file individually
        int d;
        for (d = 0; d < 15; d++){       // get the name of the file and store in namefile
            char c = arr[i][d];
                if((c>='a' && c<='z') || (c>='A' && c<='Z') || (c == ' ') || (c == '\0')){
                    nameFile[d] = c;
                }
        }
        
        // Create temporary connection variables for each file
        char* connectionOne=malloc( 15 * sizeof(char));
        char* connectionTwo=malloc( 15 * sizeof(char));
        char* connectionThree=malloc( 15 * sizeof(char));
        
        char *filepath = malloc(strlen(path) + strlen(nameFile));
        filepath = strcpy(filepath, path );
        filepath = strcat(filepath, nameFile);
        
        
        FILE* newfile;
        newfile = fopen(filepath, "w+");         //Put new files in newly created directory
        if (newfile == NULL){
            
             printf("The file could not be opened\n");
        }
        
        // We will write into the file below
        fprintf(newfile, "%s", "ROOM: ");
        fprintf(newfile, "%s\n", nameFile);
        connectionOne = makeConnection(arr, nameFile);
        fprintf(newfile, "CONNECTION1: %s\n", connectionOne);
        connectionTwo = makeConnection(arr, nameFile);
        int flag = 1;
        while (flag == 1){
        
            if (strcmp(connectionTwo, connectionOne) != 0){
                fprintf(newfile, "CONNECTION2: %s\n", connectionTwo);
                flag = 0;
            }
            else{
                connectionTwo = makeConnection(arr, filepath);
            }
        }
        
        connectionThree = makeConnection(arr, nameFile);
        flag = 1;
        while (flag == 1){
            
            if (strcmp(connectionThree, connectionTwo) != 0 && strcmp(connectionThree, connectionOne) != 0){
                fprintf(newfile, "CONNECTION3: %s\n", connectionThree);
                flag = 0;
            }
            else{
                connectionThree = makeConnection(arr, filepath);
            }
        }
        
        
        if (roomCount == 0){
            fprintf(newfile, "ROOMTYPE: Beginning\n");
        }
        else if (roomCount == 6){
            fprintf(newfile, "ROOMTYPE: Finish\n");
        }
        
        else if (roomCount != 0 && roomCount != 6){
            fprintf(newfile, "ROOMTYPE: Middle\n");
        }
    
        roomCount++;
        free(connectionOne);
        free(connectionTwo);
        free(connectionThree);
        free(filepath);
        fclose(newfile);
        int x;
        for (x = 0; x < 15; x++){
            nameFile[x] = '\0';
            
        }
            
        
    }
    
    // The files have been filled, now its time to run the game
    printf("Welcome to the CS 344 Adventure Game!\n\n");
    printf("You will start in one room, and must navigate through until you reach the end.\n");
    printf("Let's get started.\n\n\n");
    
    int pathArray[40];
    int result = printFile(0, arr, path);
    int pathCount = 0;
    
    //The printFile function sends back a -1 only when the end room has been found
    //Otherwise, it returns the number of the index of the room
    
    while (result != -1){
        pathArray[pathCount] = result;
        pathCount++;
        result = printFile(result, arr, path);
        
    }
        printf("\nYOU HAVE FOUND THE END. CONGRATS.\n");
        printf("YOU TOOK %d STEPS.\n", pathCount);
        printf("YOU VISTED THESE ROOMS: \n");
	int q;
    for (q = 0; q < pathCount; q++){
        int num = pathArray[q];
        printf("%s\n", arr[num]);
        
    }
    
 
    
}

char* makeConnection(char arr[7][15], char filename[40]){

    int r = ( rand() % 6 ) + 1;
    char* connection = malloc(15 * sizeof(char));
    int flag = 1;
    char* compare = malloc(15 * sizeof(char));
    
    
while (flag == 1){
    strcpy(compare, arr[r]);
    if (strcmp(compare, filename) != 0){
        strcpy(connection, compare);
        flag = 0;
    
        
    }
    else{
        flag = 1;
        r = ( rand() % 6 ) + 1;
    }
}
    free(compare);
    return connection;
}


int printFile(int i, char fileArray[7][15], char path[])
{
    
    char file[40];
    char buff[255];
    FILE *fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    const char s[3] = " \n";
    char *token;
    char printArray[5][15];

    int f;
    for (f = 0; f < 5; f++){
        int c;
	for (c = 0; c < 15; c++){
        
        printArray[f][c] = '\0';
        }
    }
    
    
    strcpy(file, path);
    strcat(file, fileArray[i]);
    
    char *testfile = malloc(strlen(file));
    strcpy(testfile, file);
    
    
    fp = fopen(testfile, "r");
    if (!fp){
        fprintf(stderr, "Couldn't open file");
        return 1;
    }
    
    int count = 0;
    int x = 0;
    
    // Reads the file line by line
    while ((read = getline(&line, &len, fp)) != -1) {
        
        count = 0;
        token = strtok(line, s);
        
        while (token != NULL){
            
            if (count%2 != 0){
                strcpy(printArray[x], token);
                x++;
                count++;
            }
            token = strtok(NULL, s);
            count++;
        }
        
    }
    
    if (strcmp(printArray[4], "Finish") == 0){
        return -1;
        
    }
    
    printf("\nCURRENT LOCATION: %s\n", printArray[0]);
    printf("POSSIBLE CONNECTIONS: %s, %s, %s\n", printArray[1], printArray[2], printArray[3]);
    printf("WHERE TO?>");
    scanf("%s", buff);
    int result = 0;
    int flag = 1;
    while (flag == 1){
        int r;
        for (r = 1; r < 4; r++){
            if (strcmp(buff, printArray[r]) == 0){
                result = r;
                flag = 0;
            }
        }
        if (flag != 0) {
        printf("HUH? I DON'T UNDERSTAND. PLEASE TRY AGAIN.\n");
        printf("WHERE TO?>");
        scanf("%s", buff);
        }
    }
    
    char *whereNext = printArray[result];
    int nextResult;
    int v;
    for (v = 0; v < 7; v++){
        if(strcmp(whereNext, fileArray[v]) == 0){
            nextResult = v;
            return nextResult;
        }
    }
    
    return 0;

};



