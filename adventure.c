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

// Display the congratulatory messages to the user
void displayResults(int stepCount);

int main() {
    int stepCount = 0;

    // create 7 room files, one room per file, in a directory called 
    // gilesm.rooms.<process id>

    // game commences, display user interface
    //   - player begins in starting room
    //   - game automatically ends in ending room

    // display congratulations, step count, and step history path to the user
    displayResults(stepCount);

    
    // exit with value 0
    getchar();
    return 0;
}

/******************************************************************************
 * Function Name: displayResults
 * Description: Display the congratulatory messages to the user including the
 *   total steps taken and the path traveled from start to finish.
 * Parameters:
 *   - stepCount: Integer that contains the number of steps user took from the
 *       starting room to the ending room
 * Return Type: void
 * Preconditions:
 *   - Step history file is available and readable in the appropriate 
 *       directory.
 * Postconditions: None
 *****************************************************************************/
void displayResults(int stepCount) {
    // display congratulations message
    printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
    // display number of steps taken
    printf("YOU TOOK %i STEPS. ", stepCount);
    // display path message
    printf("YOUR PATH TO VICTORY WAS: \n");
    // Read path steps from file and display them in order
    printf("Display steps from file here");
}
