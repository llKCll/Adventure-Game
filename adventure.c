//
//  connerke.adventure.c
//  adventure
//
//  Created by Kevin Conner on 10/21/17.
//  Copyright © 2017 Kevin Conner. All rights reserved.
//  Credits: 2.4 Manipulating Directories, Oregon State University

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <ctype.h>
#include <pthread.h>
#include "connerke.buildrooms.h"
#include "connerke.adventure.h"

void getRecentDir(char recentDir[])
{
    int newestDirTime = -1;
    char targetDirPrefix[35] = "connerke.rooms.";
    
    DIR *presentDir;
    struct dirent *file;
    struct stat stats;
    
    // open current directory to search for the most recent room directory
    presentDir = opendir(".");
    
    if (presentDir)
    {
        // check each dir contents
        while ((file = readdir(presentDir)) != NULL)
        {
            // check for prefix
            if (strstr(file->d_name, targetDirPrefix) != NULL)
            {
                // get struct attributes
                stat(file->d_name, &stats);
                
                // get the most recent modified time
                if ((int)stats.st_mtime > newestDirTime)
                {
                    newestDirTime = (int)stats.st_mtime;
                    memset(recentDir, '\0', 35);
                    
                    // most recent dir so far
                    sprintf(recentDir, "%s", file->d_name);
                }
            }
        }
    }
    closedir(presentDir);
}

// return true if it's a file with rooms, false otherwise
int validateFile(char fname[])
{
    if (strstr(fname, "attic") != NULL) { return 1; }
    else if (strstr(fname, "basement") != NULL) { return 1; }
    else if (strstr(fname, "bat_cave") != NULL) { return 1; }
    else if (strstr(fname, "restroom") != NULL) { return 1; }
    else if (strstr(fname, "living_room") != NULL) { return 1; }
    else if (strstr(fname, "garage") != NULL) { return 1; }
    else if (strstr(fname, "bedroom") != NULL) { return 1; }
    else if (strstr(fname, "game_room") != NULL) { return 1; }
    else if (strstr(fname, "kitchen") != NULL) { return 1; }
    else if (strstr(fname, "office") != NULL) { return 1; }
    else { return 0; }
}

// return enum value ID from the string given. return -1 if not found.
int getID(char att[])
{
    if (strcmp(att, "attic") == 0) { return attic; }
    else if (strcmp(att, "basement") == 0) { return basement; }
    else if (strcmp(att, "bat_cave") == 0) { return bat_cave; }
    else if (strcmp(att, "restroom") == 0) { return restroom; }
    else if (strcmp(att, "living_room") == 0) { return living_room; }
    else if (strcmp(att, "garage") == 0) { return garage; }
    else if (strcmp(att, "bedroom") == 0) { return bedroom; }
    else if (strcmp(att, "game_room") == 0) { return game_room; }
    else if (strcmp(att, "kitchen") == 0) { return kitchen; }
    else if (strcmp(att, "office") == 0) { return office; }
    else { return -1; }
}

void getRoomData(char recentDir[], struct Room rms[])
{
    DIR *dir;
    struct dirent *filename;
    FILE *f;
    char filePath[50];
    char line[50];
    char attribute[20];
    int rmIndex = 0;
    int spaces = 0;
    int i = 0;
    int attributeFlag = 0;
    int ID = -1;
    struct Room *currRm;
    
    dir = opendir(recentDir);
    
    if (dir)
    {
        while ((filename = readdir(dir)) != NULL)
        {
            // create full path to open
            sprintf(filePath, "%s/%s", recentDir, filename->d_name);
            
            f = fopen(filePath, "r");
           
            if (f) {
                
                // get correct file to work with
                if (validateFile(filename->d_name))
                {
                    currRm = &rms[rmIndex];
                    currRm->numConnections = 0;
                    attributeFlag = 0;
                    
                    // get line by line in file
                    while (fgets(line, 50, f) !=NULL)
                    {
                        // use spaces to know when attribute is coming
                        spaces = 0;
                        memset(attribute, '\0', sizeof(attribute));
                        
                        // use attribute flag to know which name, connection, or type
                        for (i = 0; i < 50; i++)
                        {
                            // don't copy the \n
                            if (line[i] == '\n')
                            {
                                break;
                            }
                            
                            // build line
                            if (spaces == 2)
                            {
                                sprintf(attribute, "%s%c", attribute, line[i]);
                            }
                            
                            // copy string control
                            if (line[i] == ' ')
                            {
                                spaces++;
                            }
                        }
                        
                        // make sure the attribute it's not the type, if so start a new file for next room
                        if (strcmp(attribute, "START_ROOM") == 0)
                        {
                            strcpy(currRm->type, attribute);
                            rmIndex++;
                            break;
                        }
                        
                        else if (strcmp(attribute, "MID_ROOM") == 0)
                        {
                            strcpy(currRm->type, attribute);
                            rmIndex++;
                            break;
                        }
                        
                        else if (strcmp(attribute, "END_ROOM") == 0)
                        {
                            strcpy(currRm->type, attribute);
                            rmIndex++;
                            break;
                        }
                        
                        // not type so name or connection based on flag
                        if (attributeFlag == 0 && spaces == 2)
                        {
                            ID = getID(attribute);
                            currRm->ID = ID;
                            strcpy(currRm->name, attribute);
                            attributeFlag = 1;
                        }
                        
                        else if (attributeFlag == 1 && spaces == 2)
                        {
                            ID = getID(attribute);
                            currRm->connections[currRm->numConnections] = ID;
                            currRm->numConnections++;
                        }
                    }
                }
                fclose(f);
            }
            
            else
            {
                perror("Unable to open file.");
            }
        }
    }
    
    else
    {
        perror("Unable to open directory");
    }
}

// returns the index of the start room, return -1 if not found
int getStartRoomIndex(struct Room rms[])
{
    int i;
    
    for (i = 0; i < MAX_ROOMS; i++)
    {
        if (strcmp(rms[i].type, "START_ROOM") == 0)
        {
            return i;
        }
    }
    return -1;
}

// lookup name of connection using ID and assign to room.name
void getConnectionName(struct Room *rm, int ID)
{
    if (ID == attic) { strcpy(rm->name, "attic"); }
    else if (ID == basement) { strcpy(rm->name, "basement"); }
    else if (ID == bat_cave) { strcpy(rm->name, "bat_cave"); }
    else if (ID == restroom) { strcpy(rm->name, "restroom"); }
    else if (ID == living_room) { strcpy(rm->name, "living_room"); }
    else if (ID == garage) { strcpy(rm->name, "garage"); }
    else if (ID == bedroom) { strcpy(rm->name, "bedroom"); }
    else if (ID == game_room) { strcpy(rm->name, "game_room"); }
    else if (ID == kitchen) { strcpy(rm->name, "kitchen"); }
    else if (ID == office) { strcpy(rm->name, "office"); }
}

// print current location, possible connections, where to message
void printLocation(struct Room rms[], int index)
{
    struct Room *curRm = &rms[index];
    struct Room connection;
    int i;
    
    printf("CURRENT LOCATION: %s\n", curRm->name);
    printf("POSSIBLE CONNECTIONS: ");
    
    // get location name using id
    for (i = 0; i < curRm->numConnections; i++)
    {
        getConnectionName(&connection, curRm->connections[i]);
    
        // last connection
        if (i == curRm->numConnections - 1)
        {
            printf("%s.\n", connection.name);
        }
        
        else
        {
            printf("%s, ", connection.name);
        }
    }
    printf("WHERE TO? >"); // remove \n later
}

// return true if destination is a valid room, false otherwise
int validateDestination(char dest[])
{
    if (strcmp(dest, "attic") == 0) { return 1; }
    else if (strcmp(dest, "basement") == 0) { return 1; }
    else if (strcmp(dest, "bat_cave") == 0) { return 1; }
    else if (strcmp(dest, "restroom") == 0) { return 1; }
    else if (strcmp(dest, "living_room") == 0) { return 1; }
    else if (strcmp(dest, "garage") == 0) { return 1; }
    else if (strcmp(dest, "bedroom") == 0) { return 1; }
    else if (strcmp(dest, "game_room") == 0) { return 1; }
    else if (strcmp(dest, "kitchen") == 0) { return 1; }
    else if (strcmp(dest, "office") == 0) { return 1; }
    else { return 0; }
}

// return index of destination in rms. return -1 if not found.
int getDestIndex(struct Room rms[], char dest[])
{
    int i;
    
    for (i = 0; i < MAX_ROOMS; i++)
    {
        if (strcmp(rms[i].name, dest) == 0)
        {
            return i;
        }
    }
    return -1;
}

// return true if this is the end room, false otherwise
int endRoomCheck(struct Room rms[], int index)
{
    if (strcmp(rms[index].type, "END_ROOM") == 0)
    {
        return 1;
    }
    return 0;
}

// print path taken in adventure
void printPath(int path[], int size)
{
    int i;
    struct Room rm;
    
    for (i = 0; i < size; i++)
    {
        getConnectionName(&rm, path[i]);
        printf("%s\n", rm.name);
    }
}

// display current time in separate thread
void *timeCheck(void *args)
{
    time_t t;
    struct tm *tim;
    char timStr[100];
    
    time(&t);
    tim = localtime(&t);
    strftime(timStr, 100, "%I:%M%p, %A, %B %d, %Y", tim);
    printf("\n\t%s\n\n", timStr);
    
    pthread_exit(NULL);
}

void adventure(struct Room rms[])
{
    int index;
    char destination[120];
    int steps = 0;
    int path[120] = {0};
    pthread_t thread;
    
    // find the start room
    index = getStartRoomIndex(rms);
    
    // print current location, possible connections, where to message
    printLocation(rms, index);
    
    while (1)
    {
        while (1)
        {
            // get destination from user
            scanf("%s", destination);
            
            // check if time is requested
            if (strcmp(destination, "time") == 0)
            {
                pthread_create(&thread, NULL, timeCheck, NULL);
                pthread_join(thread, NULL);
                printf("WHERE TO? >");
                continue;
            }
            
            if (!validateDestination(destination))
            {
                printf("\nHUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
                printLocation(rms, index);
                continue;
            }
            
            break;
        }
        
        // go to destination to destination until end room is found
        
        // get index of destination room
        index = getDestIndex(rms, destination);
    
        // store ID of destination in path
        path[steps] = rms[index].ID;
        steps++;
    
        // check if it's the end room
        if (endRoomCheck(rms, index))
        {
            printf("\nYOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
            printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", steps);
            printPath(path, steps);
            break;
        }
    
        printf("\n");
        printLocation(rms, index);
    }
}

int main()
{
    srand(time(NULL));
    struct Room rms[MAX_ROOMS];
    
    char recentDir[35];
    
    // get the most current directory
    getRecentDir(recentDir);
    
    // get room information from the files
    getRoomData(recentDir, rms);
    
    // start adventure
    adventure(rms);
    
    pthread_exit(NULL);
    return 0;
}
