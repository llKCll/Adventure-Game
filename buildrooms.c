//
//  connerke.buildrooms.c
//  adventure
//
//  Created by Kevin Conner on 10/21/17.
//  Copyright © 2017 Kevin Conner. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include "connerke.buildrooms.h"


void initializeRooms(struct Room rms[])
{
    int i;
    
    // set struct values
    for (i = 0; i < MAX_ROOMS; i++)
    {
        rms[i].ID = 0;
        rms[i].numConnections = 0;
        memset(rms[i].connections, '\0', sizeof(rms[i].connections));
        memset(rms[i].type, '\0', sizeof(rms[i].type));
    }
}

// return true if ID in array, false otherwise
int isIDInArray(int IDarray[], int n)
{
    int i;
    
    for (i = 0; i < MAX_ROOMS; i++)
    {
        if (IDarray[i] == n)
        {
            return 1;
        }
    }
    return 0;
}

// return true iff there is room ID in whole array, otherwise false
int isIDArrayFull(int IDarray[])
{
    int i;
    
    for (i = 0; i < MAX_ROOMS; i++)
    {
        if (IDarray[i] == 0)
        {
            return 0;
        }
    }
    return 1;
}

// append n to the id array
void putIDInArray(int IDarray[], int n)
{
    int i;
    
    for (i = 0; i < MAX_ROOMS; i++)
    {
        if (IDarray[i] == 0)
        {
            IDarray[i] = n;
            break;
        }
    }
}

// return true if graph fully formed with all connections, false otherwise
int isGraphComplete(struct Room rms[])
{
    int i;
    
    for (i = 0; i < MAX_ROOMS; i++)
    {
        if (rms[i].numConnections < MIN_CONNECTIONS)
        {
            return 0;
        }
    }
    return 1;
}

// return true if same room, false otherwise
int isSameRoom(struct Room *x, struct Room *y)
{
    if (x->ID == y->ID) {
        return 1;
    }
    return 0;
}

// return true if connection already exists, false otherwise
int connectAreadyExists(struct Room *x, struct Room *y)
{
    int i;
    
    for (i = 0; i < MAX_CONNECTIONS; i++)
    {
        if (x->connections[i] == y->ID) {
            return 1;
        }
    }
    return 0;
}

// connect two rooms together
void connectRooms(struct Room *x, struct Room *y)
{
    x->connections[x->numConnections] = y->ID;
    y->connections[y->numConnections] = x->ID;
    x->numConnections++;
    y->numConnections++;
}

// append names and IDs to struct rooms
void appendNames(struct Room rms[], int IDarray[])
{
    int i;
    int ID;
    
    for (i= 0; i < MAX_ROOMS; i++)
    {
        ID = IDarray[i];
        rms[i].ID = ID;
        
        if (ID == attic) {
            strcpy(rms[i].name, "attic");
        }
        
        else if (ID == basement) {
            strcpy(rms[i].name, "basement");
        }
        
        else if (ID == bat_cave) {
            strcpy(rms[i].name, "bat_cave");
        }
        
        else if (ID == restroom) {
            strcpy(rms[i].name, "restroom");
        }
        
        else if (ID == living_room) {
            strcpy(rms[i].name, "living_room");
        }
        
        else if (ID == garage) {
            strcpy(rms[i].name, "garage");
        }
        
        else if (ID == bedroom) {
            strcpy(rms[i].name, "bedroom");
        }
        
        else if (ID == game_room) {
            strcpy(rms[i].name, "game_room");
        }
        
        else if (ID == kitchen) {
            strcpy(rms[i].name, "kitchen");
        }
        
        else if (ID == office) {
            strcpy(rms[i].name, "office");
        }
    }
}

// assign start room and end room randomly. rest are midroom.
void assignRmTypes(struct Room rms[])
{
    int n1 = rand() % MAX_ROOMS;
    int n2 = -1;
    int i;
    
    while (1)
    {
        n2 = rand() % MAX_ROOMS;
        
        if (n1 != n2) {
            break;
        }
    }
    
    for (i = 0; i < MAX_ROOMS; i++)
    {
        if (i == n1) {
            strcpy(rms[i].type, "START_ROOM");
        }
        
        else if (i == n2) {
            strcpy(rms[i].type, "END_ROOM");
        }
        
        else {
            strcpy(rms[i].type, "MID_ROOM");
        }
    }
}

// generate struct rooms in the array for the adventure
void generateRooms(struct Room rms[])
{
    struct Room *A = malloc(sizeof(struct Room));
    struct Room *B = malloc(sizeof(struct Room));
    int HI = 10 + 1;
    int LO = 1;
    int n;
    int IDarray[MAX_ROOMS] = {0};
    
    // loop until all room are generated
    while(!isIDArrayFull(IDarray))
    {
        // get random 7/10 room ids
        n = LO + rand() % (HI - LO);
        
        // try another random room
        if(isIDInArray(IDarray, n))
        {
            continue;
        }
        
        // add room to array
        else
        {
            putIDInArray(IDarray, n);
        }
    }
    
    // append IDs and names to rooms
    appendNames(rms, IDarray);

    // assign room types randomly
    assignRmTypes(rms);

    // loop until rms are fully formed with all connections
    while(!isGraphComplete(rms))
    {
        while(1)
        {
            // select random room
            n = rand() % MAX_ROOMS;
            A = &rms[n];

            if (A->numConnections < MAX_CONNECTIONS)
            {
                break;
            }
        }

        // get second random room, validate, connect if able
        while(1)
        {
            n = rand() % MAX_ROOMS;
            B = &rms[n];

            if (B->numConnections < MAX_CONNECTIONS)
            {
                if ( (!connectAreadyExists(A, B)) && (!isSameRoom(A, B)) )
                {
                    connectRooms(A, B);
                    break;
                }
            }
        }
    }
}

// create files that hold struct content
void generateFiles(struct Room rms[])
{
    char dir[30];
    char fname[50];
    char connectionName[20];
    FILE *f;
    int i;
    int k;
    int ID;
    
    // create the directory
    sprintf(dir, "./connerke.rooms.%d", getpid());
    mkdir(dir, 0777);
    
    // create file for each room
    for (i = 0; i < MAX_ROOMS; i++)
    {
        // create the file name to open
        sprintf(fname, "%s/%s.txt", dir, rms[i].name);
        
        f = fopen(fname, "w");
        
        // input content in file
        fprintf(f, "ROOM NAME: %s\n", rms[i].name);
        
        // input connections
        for (k = 0; k < rms[i].numConnections; k++)
        {
            ID = rms[i].connections[k];
            
            if (ID == attic) {
                strcpy(connectionName, "attic");
            }
            
            else if (ID == basement) {
                strcpy(connectionName, "basement");
            }
            
            else if (ID == bat_cave) {
                strcpy(connectionName, "bat_cave");
            }
            
            else if (ID == restroom) {
                strcpy(connectionName, "restroom");
            }
            
            else if (ID == living_room) {
                strcpy(connectionName, "living_room");
            }
            
            else if (ID == garage) {
                strcpy(connectionName, "garage");
            }
            
            else if (ID == bedroom) {
                strcpy(connectionName, "bedroom");
            }
            
            else if (ID == game_room) {
                strcpy(connectionName, "game_room");
            }
            
            else if (ID == kitchen) {
                strcpy(connectionName, "kitchen");
            }
            
            else if (ID == office) {
                strcpy(connectionName, "office");
            }
            fprintf(f, "CONNECTION %d: %s\n", k+1, connectionName);
            memset(connectionName, 0, rms[i].numConnections);
        }
        fprintf(f, "ROOM TYPE: %s\n", rms[i].type);
    }
}

int main()
{
    srand(time(NULL));
    struct Room rms[7];
    initializeRooms(rms);
    
    // build rooms and place in rms
    generateRooms(rms);
    
    // create files with struct info
    generateFiles(rms);
    
    
    
    
    return 0;
}




