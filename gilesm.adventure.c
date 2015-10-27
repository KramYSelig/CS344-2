/******************************************************************************
 * Author: Mark Giles
 * Filename: adventure.c
 * Date Created: 10/22/2015
 * Date Last Modified: 10/22/2015
 * Description: This is an adventure game that creates 7 random rooms, creates
 *   a starting room, an ending room, up to five rooms in between, and the
 *   connections between rooms. A user will be placed in the starting room, and
 *   use the provided interface to navigate between rooms. Once a user reaches
 *   the ending room, the game is over and the user wins. Upon completion, the
 *   program will display a message to the user, and indicate the number of
 *   steps taken as well as display the path taken by room name.
 *
 * Input: The program will read data from files that contain room information
 *   as well as receive input from the user through standard input (keyboard)
 *   for navigation.
 *
 * Output: The program will write randomly selected room data to files to
 *   create a dynamically gaming experience and will also use standard output
 *   (display to the screen) for the user interface and communication.
 *
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

struct Room {
	char name[50];						// label/name
	char type[10];						// START_ROOM, END_ROOM, MID_ROOM
	int numActiveConns;					// number of room's current connections
	int connections[7];					// rooms that this room connects to
};

struct Game {
	struct Room roomList[7];			// room information for this game
	char nameList[10][50];				// list of available names
	int numNamesRemaining;				// number of names remaining in list
	int stepCount;						// tracks number of steps taken
	int startRoomAssigned;				// tracks if start room is assigned
	int endRoomAssigned;				// tracks if end room is assigned
	int processID;
	char dirPath[512];
};

// takes a game structure, a room index, and adds a connection to that room
void addRoomConnection(struct Game *currentGame, int roomIndex);
// assign room names, room connections, and save room files to directory
void buildGame(struct Game *currentGame);
// clean game structure memory
void cleanGameData(struct Game *currentGame);
// Display the congratulatory messages to the user
void displayGameResults(struct Game *currentGame);
// Retrieve random name from names list and decrement number remaining by 1
void getRandomName(struct Game *currentGame, char *name);
// initialize the game attributes, room name list, and directory path
void initGame(struct Game *currentGame);
// allow player to play game until end room is reached
void playGame(struct Game *currentGame);

int main() {
	struct Game *currentGame;
	currentGame = (struct Game *)malloc(sizeof(struct Game));
	
	// initialize the game attributes, room name list, and directory path
	initGame(currentGame);
	// assign room names, room connections, and save room files to directory
	buildGame(currentGame);
	// allow player to play game until end room is reached
	playGame(currentGame);
    // display congratulations, step count, and step history path to the user
	displayGameResults(currentGame);
	// clean game structure memory
	cleanGameData(currentGame); 
    // exit with value 0
    return 0;
}

/******************************************************************************
 * Function Name: buildGame
 * Description: Assign room names, room connections, and save room files to
 *   directory.
 *****************************************************************************/
void buildGame(struct Game *currentGame) {
	int status,					// success/fail for directory and file creation
		i = 0,					// iterator for control structures
		j = 0,					// iterator for control structures
		numConnectionsToAdd;	// number of connections selected for this room

	// stores random name selection from list
	char *roomName = malloc(sizeof(char) * 50); 		

	// initialize seed for random number generator
	srand (time(NULL));

	// create room file directory
	status = mkdir(currentGame->dirPath, 0775);
	// create room files while assigning name, type, and connections
	for (i = 0; i < 7; i++) {
		// select random name from name list
		getRandomName(currentGame, roomName);
		strcpy(currentGame->roomList[i].name, roomName);
		// calculate random number of connections assigned to this room
		// generate random number between 3 and 6
		numConnectionsToAdd = ((rand() % 4) + 3);
		// subtract number of connections this room already has
		numConnectionsToAdd -= currentGame->roomList[i].numActiveConns;
		
		// add connections until assigned number of connections is reached
		for (j = 1; j <= numConnectionsToAdd; j++) {
			addRoomConnection(currentGame, i);
		}
		
	}
	
	printf("\n");
	for (i = 0; i < 7; i++) {
		printf("Room Index Number: %i\n", i);
		printf("Room Name: %s\n", currentGame->roomList[i].name);
		printf("Number of connections: %i\n", currentGame->roomList[i].numActiveConns);
		for (j = 0; j < 7; j++) {
			if (currentGame->roomList[i].connections[j] == 1 && i != j) {
				printf("Connection %i: %s\n", j, currentGame->roomList[j].name);
			}
		}
		printf("\n");
	}

	// read room files into local game room structures
}

/******************************************************************************
 * Function Name: addRoomConnection
 * Description: Takes a game structure and a specfied room and adds to it a
 *   connection to another room.
 *****************************************************************************/
void addRoomConnection(struct Game *currentGame, int roomIndex) {
	int connectionEstablished = 0,
		randomSelection = -1;
	while (connectionEstablished == 0) {
		// generate random number between 0 and 6
		randomSelection = (rand() % 7);
		
		if (currentGame->roomList[roomIndex].connections[randomSelection] == 0) {
			currentGame->roomList[roomIndex].connections[randomSelection] = 1;
			connectionEstablished = 1;
		}
	}
	currentGame->roomList[roomIndex].numActiveConns++;
}

/******************************************************************************
 * Function Name: getRandomName
 * Description: Selects a random name from the game list, removes the name, and
 *   updates the total number of names remaining.
 *****************************************************************************/
void getRandomName(struct Game *currentGame, char *name) {
	int i = 0,				// iterator for control structures
		randomNum = 0;		// random number between 0 and last element in array
	
	// generate random number to select from names list
	randomNum = rand() % currentGame->numNamesRemaining;
	// copy name from name list at index specified by the random number
	strcpy(name, currentGame->nameList[randomNum]);
	// remove the selected name from the list to avoid selecting it again
	for (i = randomNum; i < currentGame->numNamesRemaining; i++) {
		strcpy(currentGame->nameList[i], currentGame->nameList[i + 1]);
	}
	// decrement the number of names remaining in the list by 1
	currentGame->numNamesRemaining--;
}


/******************************************************************************
 * Function Name: cleanGameData
 * Description: Clean game structure memory.
 *****************************************************************************/
void cleanGameData(struct Game *currentGame) {

}


/******************************************************************************
 * Function Name: displayGameResults
 * Description: Display the congratulatory messages to the user including the
 *   total steps taken and the path traveled from start to finish.
 *****************************************************************************/
void displayGameResults(struct Game *currentGame) {
    // display congratulations message
    printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
    // display number of steps taken
    printf("YOU TOOK %i STEPS. ", currentGame->stepCount);
    // display path message
    printf("YOUR PATH TO VICTORY WAS: \n");
    // Read path steps from file and display them in order
    printf("Display steps from file here\n");
}

/******************************************************************************
 * Function Name: initGame
 * Description: Initialize the game attributes, room name list, and directory
 *   path.
 *****************************************************************************/
void initGame(struct Game *currentGame) {
	int i = 0;
	char buffer[512];

	// initialize name list with 10 predefined options
	strcpy(currentGame->nameList[0], "Lila's Room");
	strcpy(currentGame->nameList[1], "Lila's Cell");
	strcpy(currentGame->nameList[2], "Mother's Secrect Office");
	strcpy(currentGame->nameList[3], "Kitchen");
	strcpy(currentGame->nameList[4], "Old Torture Room");
	strcpy(currentGame->nameList[5], "Rooftop Deck");
	strcpy(currentGame->nameList[6], "Master Bedroom");
	strcpy(currentGame->nameList[7], "Dark Closet");
	strcpy(currentGame->nameList[8], "New Torture Room");
	strcpy(currentGame->nameList[9], "Dining Room");
	currentGame->numNamesRemaining = 10;

	// initialize basic parameters
	currentGame->stepCount = 0;				// number of steps taken
	currentGame->startRoomAssigned = 0;		// tracks if start room is assigned
	currentGame->endRoomAssigned = 0;		// tracks if end room is assigned
	currentGame->numNamesRemaining = 10;	// number of names available
	for (i = 0; i < 7; i++) {
		currentGame->roomList[i].numActiveConns = 0;
		// establish connection to self
		currentGame->roomList[i].connections[i] = 1;
	}


	// gather current process id and build directory path for files
	currentGame->processID = getpid();		// current process ID for program
	sprintf(buffer,"gilesm.rooms.%d", currentGame->processID); 
	strcpy(currentGame->dirPath, buffer);
}

/******************************************************************************
 * Function Name: playGame
 * Description: Allow the player to play game until end room is reached.
 *****************************************************************************/
void playGame(struct Game *currentGame) {

}

