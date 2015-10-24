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
// included libraries
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

struct Room {
	char name[50];						// label/name
	char type[10];						// START_ROOM, END_ROOM, MID_ROOM
	int numActiveConns;					// number of room's current connections
	int connections[6];					// rooms that this room connects to
};

struct Game {
	struct Room roomList[7];			// room information for this game
	char nameList[10][50];				// list of available names
	int stepCount;						// tracks number of steps taken
	int startRoomAssigned;				// tracks if start room is assigned
	int endRoomAssigned;				// tracks if end room is assigned
};

// initialize the game attributes and room values
void initGame(struct Game *currentGame);
// Display the congratulatory messages to the user
void displayResults(struct Game *currentGame);

int main() {
	struct Game *currentGame;
	currentGame = (struct Game *)malloc(sizeof(struct Game));
	
	// initialize game attributes, set room values, and save rooms to file
	initGame(currentGame);

    // display congratulations, step count, and step history path to the user
	displayResults(currentGame);
 
    // exit with value 0
    return 0;
}

/******************************************************************************
 * Function Name: displayResults
 * Description: Display the congratulatory messages to the user including the
 *   total steps taken and the path traveled from start to finish.
 *****************************************************************************/
void displayResults(struct Game *currentGame) {
    // display congratulations message
    printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
    // display number of steps taken
    printf("YOU TOOK %i STEPS. ", currentGame->stepCount);
    // display path message
    printf("YOUR PATH TO VICTORY WAS: \n");
    // Read path steps from file and display them in order
    printf("Display steps from file here");
	printf("%s", currentGame->nameList[0]);
}

/******************************************************************************
 * Function Name: initGame
 * Description: Initialize the game includeing attributes and room information
 *****************************************************************************/
void initGame(struct Game *currentGame) {
	// initialize name list with 10 predefined options
	currentGame->nameList[0] = "test";
	// initialize basic parameters
	currentGame->stepCount = 0;				// tracks number of steps taken
	currentGame->startRoomAssigned = 0;		// tracks if start room is assigned
	currentGame->endRoomAssigned = 0;		// tracks if end room is assigned
}
