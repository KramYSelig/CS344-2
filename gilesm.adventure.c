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
	char type[15];						// START_ROOM, END_ROOM, MID_ROOM
	int numActiveConn;					// number of current connections
	int connections[7];					// rooms that this room connects to
};

struct Game {
	struct Room roomList[7];			// room information for this game
	char nameList[10][50];				// list of available names
	int numNamesRemaining;				// number of names remaining in list
	int stepCount;						// tracks number of steps taken
	int processID;						// process ID of current game
	char dirPath[512];					// path of directory for files
	int startRoomIndex;					// room list index of starting room
	int endRoomIndex;					// room list index of ending room
};

// takes a game structure, a room index, and adds a connection to that room
void addRoomConn(struct Game *currentGame, int roomIndex);
// assign room names, room connections, and save room files to directory
void buildGame(struct Game *currentGame);
// populate room files with description/information for a specified room number
void writeRoomFile(struct Game *currentGame, int roomNumber);
// read room file contents into local structure for a specified room number
void readRoomFile(struct Game *currentGame, int roomNumber);
// create a room file for the game with a specified file number.
void createRoomFile(struct Game *currentGame, int roomNumber);
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
	// initialize seed for random number generator
	srand (time(NULL));
	
	// initialize the game attributes, room name list, and directory path
	initGame(currentGame);
	// assign room names, room connections, and save room files to directory
	buildGame(currentGame);
	// allow player to play game until end room is reached
	playGame(currentGame);
    // display congratulations, step count, and step history path to the user
	displayGameResults(currentGame);
	
	// clean game structure memory
	free(currentGame); 
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
		numConnToAdd;			// number of connections selected for this room
	char *fileName = malloc(sizeof(char) * 10);
	// stores random name selection from list
	char *roomName = malloc(sizeof(char) * 50);
		
	
		// create room files while assigning name, type, and connections
	for (i = 0; i < 7; i++) {
		// select random name from name list
		getRandomName(currentGame, roomName);
		// save random name to room structure
		strcpy(currentGame->roomList[i].name, roomName);
	
		// add connections to this room until it has at least 3
		while (currentGame->roomList[i].numActiveConn < 3) {
			addRoomConn(currentGame, i);
		}

		// assign a room type based on random selections
		if (currentGame->startRoomIndex == i) {
			strcpy(currentGame->roomList[i].type, "START_ROOM");
		} else if (currentGame->endRoomIndex == i) {
			strcpy(currentGame->roomList[i].type, "END_ROOM");
		} else {
			strcpy(currentGame->roomList[i].type, "MID_ROOM");
		}
	}

		
	// populate room files with description/information
	for (i = 0; i < 7; i++) {
		writeRoomFile(currentGame, i);
	}

	// read room file contents into local game structure
	for (i = 0; i < 7; i++) {
		readRoomFile(currentGame, i);
	}
	
}

/******************************************************************************
 * Function Name: writeRoomFile
 * Description: Populate room file with description/information for a specified
 *   file number.
 *****************************************************************************/
void writeRoomFile(struct Game *currentGame, int roomNumber) {
	char *fileName = malloc(sizeof(char) * 10);
	char *roomName = malloc(sizeof(char) * 61);
	char *roomType = malloc(sizeof(char) * 26);
	char *connectionRoomName = malloc(sizeof(char) * 64);
	int file_descriptor;
	int i = 0;
	int connectionNumber = 1;
	ssize_t nwritten;

	// concatenate text with data to be written to the file
	sprintf(fileName, "%s/file%d", currentGame->dirPath, roomNumber);
	sprintf(roomName, "ROOM NAME: %s\n", currentGame->roomList[roomNumber].name);
	sprintf(roomType, "ROOM TYPE: %s\n", currentGame->roomList[roomNumber].type);

	file_descriptor = open(fileName, O_RDWR);
	if (file_descriptor == -1) {
		fprintf(stderr, "Could not open %s to write to file.\n", fileName);
		exit(1);
	}

	nwritten = write(file_descriptor, roomName, strlen(roomName));
	for (i = 0; i < 7; i++) {
		if (currentGame->roomList[roomNumber].connections[i] == 1 && roomNumber != i) {
			sprintf(connectionRoomName, "CONNECTION %i: %s\n", connectionNumber, currentGame->roomList[i].name);
			nwritten = write(file_descriptor, connectionRoomName, strlen(connectionRoomName));
			connectionNumber++;
		}
	}

	nwritten = write(file_descriptor, roomType, strlen(roomType));

	close(file_descriptor);

	fileName = NULL;
	roomName = NULL;
	roomType = NULL;
	connectionRoomName = NULL;
	free(fileName);
	free(roomName);
	free(roomType);
	free(connectionRoomName);
}

/******************************************************************************
 * Function Name: readRoomFile
 * Description: Read room file contents into local game structure for a
 *   specified room number.
 *****************************************************************************/
void readRoomFile(struct Game *currentGame, int roomNumber) {
	char *buffer = malloc(sizeof(char) * 512);
	char *fileName = malloc(sizeof(char) * 10);
	char *roomName = malloc(sizeof(char) * 61);
	char *roomType = malloc(sizeof(char) * 26);
	char *connectionRoomName = malloc(sizeof(char) * 64);
	int file_descriptor;
	int i = 0;
	int readPos = 0;
	ssize_t nwritten;
	ssize_t nread;
	size_t len = 0;
	sprintf(fileName, "%s/file%d", currentGame->dirPath, roomNumber);
	file_descriptor = open(fileName, O_RDONLY);
	if (file_descriptor < 0) {
		fprintf(stderr, "Could not open %s to read the file.\n", fileName);
		exit(1);
	}
	
	nread = getline(&buffer, &len, file_descriptor);

	//readPos = lseek(file_descriptor, 11, SEEK_SET);
	//nread = read(file_descriptor, buffer, 512);
	printf("\nNew File:\n\n");
	printf("%s", buffer);

	close(file_descriptor);
}

/******************************************************************************
 * Function Name: createRoomFile
 * Description: Create a room file for the game with a specified file number.
 *****************************************************************************/
void createRoomFile(struct Game *currentGame, int roomNumber) {
	char *fileName = malloc(sizeof(char) * 10);
	int file_descriptor;
	sprintf(fileName, "%s/file%d", currentGame->dirPath, roomNumber);
	file_descriptor = open(fileName, O_RDONLY | O_CREAT, 0775);
	if (file_descriptor < 0) {
		fprintf(stderr, "Could not open %s to create the file.\n", fileName);
		exit(1);
	}

	close(file_descriptor);

	fileName = NULL;
	free(fileName);
}

/******************************************************************************
 * Function Name: addRoomConn
 * Description: Takes a game structure and a specfied room and adds to it a
 *   connection to another room.
 *****************************************************************************/
void addRoomConn(struct Game *currentGame, int roomIndex) {
	int connectionEstablished = 0,
		randomSelection = -1;
	
	while (connectionEstablished == 0) {
		// generate random number between 0 and 6
		randomSelection = (rand() % 7);
		if (currentGame->roomList[roomIndex].connections[randomSelection] == 0) {
				currentGame->roomList[roomIndex].connections[randomSelection] = 1;
				currentGame->roomList[randomSelection].connections[roomIndex] = 1;
				currentGame->roomList[roomIndex].numActiveConn++;
				currentGame->roomList[randomSelection].numActiveConn++;
				connectionEstablished = 1;
		}
	}
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
	int i = 0,
		status;
	char buffer[512];

	// initialize name list with 10 predefined options
	strcpy(currentGame->nameList[0], "Lila's Room");
	strcpy(currentGame->nameList[1], "Lila's Cell");
	strcpy(currentGame->nameList[2], "Mother's Secret Office");
	strcpy(currentGame->nameList[3], "Kitchen");
	strcpy(currentGame->nameList[4], "Old Torture Room");
	strcpy(currentGame->nameList[5], "Rooftop Deck");
	strcpy(currentGame->nameList[6], "Master Bedroom");
	strcpy(currentGame->nameList[7], "Dark Closet");
	strcpy(currentGame->nameList[8], "New Torture Room");
	strcpy(currentGame->nameList[9], "Dining Room");
	// all names are remaining initially
	currentGame->numNamesRemaining = 10;

	// initialize basic parameters
	currentGame->stepCount = 0;				// number of steps taken
	currentGame->numNamesRemaining = 10;	// number of names available
	currentGame->startRoomIndex = -1;		// start room index, -1 is bad
	currentGame->endRoomIndex = -1;			// end room index, -1 is bad
	
	// randomly select starting room
	currentGame->startRoomIndex = (rand() % 7);
	// randomly select ending room ensuring it is different from start room
	do {
		currentGame->endRoomIndex = (rand() % 7);
	} while (currentGame->startRoomIndex == currentGame->endRoomIndex);

	// initialize each room active connections to 0, add connection to self
	for (i = 0; i < 7; i++) {
		// 0 initial active connections
		currentGame->roomList[i].numActiveConn = 0;
		// establish connection to self, does not add to active connections
		currentGame->roomList[i].connections[i] = 1;
	}


	// gather current process id and build directory path for files
	currentGame->processID = getpid();		// current process ID for program
	sprintf(buffer,"gilesm.rooms.%d", currentGame->processID); 
	strcpy(currentGame->dirPath, buffer);
	// create room file directory
	status = mkdir(currentGame->dirPath, 0775);
	// create room files with appropriate permissions
	for (i = 0; i < 7; i++) {
		createRoomFile(currentGame, i);
	}
}

/******************************************************************************
 * Function Name: playGame
 * Description: Allow the player to play game until end room is reached.
 *****************************************************************************/
void playGame(struct Game *currentGame) {

}
