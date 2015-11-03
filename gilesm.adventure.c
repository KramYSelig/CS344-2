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
	char stepFileName[512];				// name of step history file 
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
	// initialize game structure and allocate memory
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
	// clean game data
	remove(currentGame->stepFileName);
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
	char *fileName = malloc(sizeof(char) * 10),
		 *roomName = malloc(sizeof(char) * 50);
		
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
	free(fileName);
	free(roomName);
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

	// open file for writing
	file_descriptor = open(fileName, O_RDWR);
	// check to see if file opened successfully
	if (file_descriptor == -1) {
		fprintf(stderr, "Could not open %s to write to file.\n", fileName);
		exit(1);
	}
	// write the room name to the file
	nwritten = write(file_descriptor, roomName, strlen(roomName));
	// write each connecting room name to file
	for (i = 0; i < 7; i++) {
		if (currentGame->roomList[roomNumber].connections[i] == 1 && roomNumber != i) {
			sprintf(connectionRoomName, "CONNECTION %i: %s\n", connectionNumber, currentGame->roomList[i].name);
			nwritten = write(file_descriptor, connectionRoomName, strlen(connectionRoomName));
			connectionNumber++;
		}
	}
	// write the room type to the file
	nwritten = write(file_descriptor, roomType, strlen(roomType));
	// close the file
	close(file_descriptor);
	// clean up pointers
	fileName = NULL;
	roomName = NULL;
	roomType = NULL;
	connectionRoomName = NULL;
	// restore dynamic memory
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
	char buffer[512] = "",
	     fileName[10] = "",
	     roomName[50] = "",
	     roomType[15] = "",
	     connectionRoomName[50] = "",
	     c,
	     subBuff[10];
	int count = 0,
	    i = 0,
	    index = 0;
	// concatenate directory path and file name
	sprintf(fileName, "%s/file%d", currentGame->dirPath, roomNumber);
	// open file pointer for reading
	FILE *fp = fopen(fileName, "r");
	// read each character of the file until the end
	while ((c = fgetc(fp)) !=  EOF) {
		if (c == '\n') {
			buffer[count] = c;
			// determine which type of row is being read
			for (i = 0; i < 10; i++) {
				subBuff[i] = buffer[i];
			}
			// store appropriate value based on what the row holds
			if (strcmp(subBuff, "ROOM NAME:") == 0) {
				index = 11;
				count = 0;
				while (count < 50) {
					if (buffer[index] == '\n') {
						buffer[index] = '\0';
					} else {
						roomName[count] = buffer[index];
					}
					count++;
					index++;
				}
				strcpy(currentGame->roomList[roomNumber].name, roomName);
			} else if (strcmp(subBuff, "CONNECTION") == 0) {
				index = 14;
				count = 0;
				while (count < 50) {
					if (buffer[index] == '\n') {
						buffer[index] = '\0';
					} else {
						connectionRoomName[count] = buffer[index];
					}
					count++;
					index++;
				}
				
				for ( i = 0; i < 7; i++) {
					if (strcmp(connectionRoomName, currentGame->roomList[i].name) == 0) {
						currentGame->roomList[roomNumber].connections[i] = 1;
					}
				}
				for (i = 0; i < 50; i++) {
					connectionRoomName[i] = '\0';
				}
			} else if (strcmp(subBuff, "ROOM TYPE:") == 0) {
				index = 11;
				count = 0;
				while (count < 15) {
					if (buffer[index] == '\n') {
						buffer[index] = '\0';
					} else {
						roomType[count] = buffer[index];
					}
					count++;
					index++;
				}
				strcpy(currentGame->roomList[roomNumber].type, roomType);
			}
			// clear buffer variable for re-use
			for (i = 0; i < 512; i++) {
				buffer[i] = '\0';
			}
			count = 0;
		} else {
			// if not newline, store character and add to total characters read
			buffer[count] = c;
			count++;
		}
	}
	// close the file	
	fclose(fp);
}

/******************************************************************************
 * Function Name: createRoomFile
 * Description: Create a room file for the game with a specified file number.
 *****************************************************************************/
void createRoomFile(struct Game *currentGame, int roomNumber) {
	char *fileName = malloc(sizeof(char) * 10);
	int file_descriptor;
	sprintf(fileName, "%s/file%d", currentGame->dirPath, roomNumber);
	// open and create file
	file_descriptor = open(fileName, O_RDONLY | O_CREAT, 0775);
	if (file_descriptor < 0) {
		fprintf(stderr, "Could not open %s to create the file.\n", fileName);
		exit(1);
	}
	// close the file
	close(file_descriptor);
	// free pointer
	fileName = NULL;
	// free dynamic memory
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
	
	// search for random connection match until established
	while (connectionEstablished == 0) {
		// generate random number between 0 and 6
		randomSelection = (rand() % 7);
		// add connection if not at max and if connection doesn't already exist
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
	FILE *fp = fopen(currentGame->stepFileName, "r");
	char c;
	// display congratulations message
    printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
    // display number of steps taken
    printf("YOU TOOK %i STEPS. ", currentGame->stepCount);
    // display path message
    printf("YOUR PATH TO VICTORY WAS: \n");

    // Read path steps from file and display them in order
	while ((c = fgetc(fp)) != EOF) {
		printf("%c", c);
	}

	fclose(fp);
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
	int file_descriptor;
	
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
	// create step list temporary file
	sprintf(currentGame->stepFileName, "%s/stepHistory", currentGame->dirPath);
	file_descriptor = open(currentGame->stepFileName, O_RDONLY | O_CREAT, 0775);
	if (file_descriptor < 0) {
		fprintf(stderr, "Could not open %s to create the file.\n", currentGame->stepFileName);
		exit(1);
	}
	close(file_descriptor);
}

/******************************************************************************
 * Function Name: playGame
 * Description: Allow the player to play game until end room is reached.
 *****************************************************************************/
void playGame(struct Game *currentGame) {
	int currentLocation = currentGame->startRoomIndex,
		i = 0,
		count = 0,
		success = 0,
		file_descriptor;
	char buffer[50];
	char *fileName = malloc(sizeof(char) * 25);
	char *historyName = malloc(sizeof(char) * 50);
	ssize_t nwritten;
	// open step history file for writing
	file_descriptor = open(currentGame->stepFileName, O_RDWR);
	if (file_descriptor == -1) {
		fprintf(stderr, "Could not open %s to write to file.\n", fileName);
		exit(1);
	}
	// allow player to move through connected rooms until end room is reached
	while (currentLocation != currentGame->endRoomIndex) {
		// allows different formatting for first connection
		count = 0;
		// determines if user typed appropriate connection room name
		success = 0;
		// shows room name for current room
		printf("CURRENT LOCATION: %s\n", currentGame->roomList[currentLocation].name);
		// displays room name for all connected rooms
		printf("POSSIBLE CONNECTIONS: ");
		for (i = 0; i < 7; i++) {
			if (currentGame->roomList[currentLocation].connections[i] == 1 && currentLocation != i) {
				if (count == 0) {
					printf("%s", currentGame->roomList[i].name);
					count++;
				} else {
					printf(", %s", currentGame->roomList[i].name);
				}
			}
		}
		printf(".\n");
		printf("WHERE TO? >");
		// gets user input for room selection
		fgets(buffer, 50, stdin);
		// terminates string with null character
		buffer[strlen(buffer) - 1] = '\0';
		// reads through each room to retrieve room names
		for (i = 0; i < 7; i++) {
			if (currentGame->roomList[currentLocation].connections[i] == 1 && currentLocation != i) {
				if (strcmp(currentGame->roomList[i].name, buffer) == 0) {
					currentLocation = i;
					sprintf(historyName, "%s\n", currentGame->roomList[i].name);
					// stores room name for history
					nwritten = write(file_descriptor, historyName, strlen(historyName));
					// increments total step count
					currentGame->stepCount++;
					// indicates user typed successful connecting room name
					success = 1;
				}
			}
		}
		printf("\n");
		if (success == 0) {
			printf("HUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
			printf("\n");
		}
	}
	close(file_descriptor);
}
