

#ifndef connerke_buildrooms_h
#define connerke_buildrooms_h

#define MAX_CONNECTIONS 6
#define MIN_CONNECTIONS 3
#define MAX_ROOMS 7

enum rooms { attic=1, basement, bat_cave, restroom,
             living_room, garage, bedroom, game_room,
             kitchen, office
};

struct Room
{
    int ID;
    char name[20];
    int numConnections;
    char type[20];
    int connections[MAX_CONNECTIONS];
};

void initializeRooms(struct Room rms[]);
int isIDInArray(int IDarray[], int n);
int isIDArrayFull(int IDarray[]);
void putIDInArray(int IDarray[], int n);
int isGraphComplete(struct Room rms[]);
int isSameRoom(struct Room *x, struct Room *y);
int connectAreadyExists(struct Room *x, struct Room *y);
void connectRooms(struct Room *x, struct Room *y);
void appendNames(struct Room rms[], int IDarray[]);
void assignRmTypes(struct Room rms[]);
void generateRooms(struct Room rms[]);
void generateFiles(struct Room rms[]);


#endif /* connerke_buildrooms_h */
