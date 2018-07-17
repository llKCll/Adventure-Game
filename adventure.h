//
//  connerke.adventure.h
//  adventure
//
//  Created by Kevin Conner on 10/21/17.
//  Copyright © 2017 Kevin Conner. All rights reserved.
//

#ifndef connerke_adventure_h
#define connerke_adventure_h


void getRecentDir(char []);
void getRoomData(char recentDir[], struct Room rms[]);
int validateFile(char []);
int getID(char []);
void adventure(struct Room []);
int getStartRoomIndex(struct Room []);
void printLocation(struct Room [], int);
void getConnectionName(struct Room *, int);
int validateDestination(char []);
int getDestIndex(struct Room [], char []);
int endRoomCheck(struct Room [], int);
void printPath(int [], int);
void *timeCheck(void *);

#endif /* connerke_adventure_h */
