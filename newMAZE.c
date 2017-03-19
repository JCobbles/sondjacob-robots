
#include "simpletools.h"
#include "ping.h"
#include "abdrive.h"
#include <stdio.h>

#define true 1
#define false 0

#define NEVER 0
#define ONCE 1
#define TWICE 2
#define CORRIDOR 3

#define NORTH 0
#define WEST 1
#define EAST 2
#define SOUTH 3

#define MAX_SPEED 128 // in ticks / sec
#define FWD 420

int currentDirection;

typedef struct Square {
    int x;
    int y;
    int visited;
    struct Square* north;
    struct Square* west;
    struct Square* east;
    struct Square* south;
} Square;

double radius = 52.9;

int newRound(double x) {
    if (x - (int) x < 0.5) return (int) x;
    else return (int) x + 1;
}

int calculateTicks(int distanceInMillimetres) {
    return newRound(distanceInMillimetres / 3.25);
}

void turn(int degrees) {
    if (degrees == 90) {
        drive_goto(26, -25);
    } else if (degrees == -90) {
        drive_goto(-25, 26);
    } else if (degrees == 180) {
        drive_goto(52, -50);
    } else {
        double radians = degrees *  PI / 180;
        int ticks = calculateTicks(radians * radius);
        drive_goto(ticks, -ticks);
    }
}

void forwards(int distance) {
    drive_goto(calculateTicks(distance), calculateTicks(distance));
}

Square* current_pos, start_pos;

int reverseDirection(int direction) {
    return (direction - 2) % 4;
}

int calculateCardinalDirection(int localDirection) {
    if (currentDirection == NORTH) return localDirection;
    else if (localDirection == NORTH) return currentDirection;
    
    else if (currentDirection == WEST) {
        switch (localDirection) {
            case WEST: return SOUTH;
            case EAST: return NORTH;
            case SOUTH: return EAST;
        }
    } else if (currentDirection == EAST) {
        switch (localDirection) {
            case WEST: return NORTH;
            case EAST: return SOUTH;
            case SOUTH: return WEST;
        }
    } else if (currentDirection == SOUTH) {
        switch (localDirection) {
            case WEST: return EAST;
            case EAST: return WEST;
            case SOUTH: return NORTH;
        }
    }
}

int isRightDirection(Square* square) {
    return square->visited == ONCE || square->visited == CORRIDOR;
}

void move(int direction) {
    forwards(40);
    currentDirection = direction;
    switch (direction) {
        case NORTH:
            break;
        case EAST:
            break;
        case SOUTH:
            break;
        case WEST:
            break;
    }
}

void returnJourney() {
    while (true) {
        if (isRightDirection(current_pos->south)) {
            current_pos = current_pos->south;
            move(SOUTH);
        } else if (isRightDirection(current_pos->west)) {
            current_pos = current_pos->west;
            move(WEST);
        } else if (isRightDirection(current_pos->north)) {
            current_pos = current_pos->north;
            move(NORTH);
        } else if (isRightDirection(current_pos->east)) {
            current_pos = current_pos->east;
            move(EAST);
        }
    }
}

int direction_to_move, num_free_paths;

Square* getSquare(int x, int y) {
    /*Square* crawler = start_pos;
    while (crawler != NULL) {
        
    } */
}

void analyseSquare(Square* current_pos, int localDirection, int wall_distance) {
    if (wall_distance < 10) {
        move(-20); // back off a little if wall is too close
    }
    int globalDirection = calculateCardinalDirection(localDirection);
    switch (globalDirection) {
        case NORTH:
            if (wall_distance < 40) {
                current_pos->north = NULL;
                printf("Detected wall to the north\n");
                return;
            }
            if (current_pos->north == NULL) {
                current_pos->north = calloc(1, sizeof(Square));
                current_pos->north->x = current_pos->x;
                current_pos->north->y = current_pos->y + 1;
                current_pos->north->visited = NEVER;
                direction_to_move = localDirection;
                printf("Recorded unvisited square to the north\n");
            }
            else if (current_pos->north->visited == NEVER || current_pos->north->visited == CORRIDOR) {
                direction_to_move = localDirection;
                printf("Relocated corridor or unvisited square to the north\n");
            }
            num_free_paths++;
            return;
        case WEST:
            if (wall_distance < 40) {
                current_pos->west= NULL;
                printf("Detected wall to the west\n");
                return;
            }
            if (current_pos->west == NULL) {
                current_pos->west = calloc(1, sizeof(Square));
                current_pos->west->x = current_pos->x - 1;
                current_pos->west->y = current_pos->y;
                current_pos->west->visited = NEVER;
                direction_to_move = localDirection;
                printf("Recorded unvisited square to the west\n");
            }
            else if (current_pos->west->visited == NEVER || current_pos->west->visited == CORRIDOR) {
                direction_to_move = localDirection;
                printf("Relocated corridor or unvisited square to the west\n");
            }
            num_free_paths++;
            return;
        case EAST:
            if (wall_distance < 40) {
                current_pos->east= NULL;
                printf("Detected wall to the east\n");
                return;
            }
            if (current_pos->east == NULL) {
                current_pos->east = calloc(1, sizeof(Square));
                current_pos->east->x = current_pos->x + 1;
                current_pos->east->y = current_pos->y;
                current_pos->east->visited = NEVER;
                direction_to_move = localDirection;
                printf("Recorded unvisited square to the east\n");
            }
            else if (current_pos->east->visited == NEVER || current_pos->east->visited == CORRIDOR) {
                direction_to_move = localDirection;
                printf("Relocated corridor or unvisited square to the east\n");
            }
            num_free_paths++;
            return;  
        case SOUTH:
            if (wall_distance < 40) {
                current_pos->south = NULL;
                printf("Detected wall to the south\n");
                return;
            }
            if (current_pos->south == NULL) {
                current_pos->south = calloc(1, sizeof(Square));
                current_pos->south->x = current_pos->x;
                current_pos->south->y = current_pos->y - 1;
                current_pos->south->visited = NEVER;
                direction_to_move = localDirection;
                printf("Recorded unvisited square to the south\n");
            }
            else if (current_pos->south->visited == NEVER || current_pos->south->visited == CORRIDOR) {
                direction_to_move = localDirection;
                printf("Relocated corridor or unvisited square to the south\n");
            }
            num_free_paths++;
            return; 
    }
}

int main() {
    low(26);  
    low(27);
    forwards(FWD);
    start_pos = calloc(1, sizeof(Square));
    start_pos->x = 0;
    start_pos->y = 0;
    start_pos->visited = ONCE;
    current_pos = start_pos;
    currentDirection = NORTH;    
        
    while (true) {
        printf("\nEntered square (%d, %d)\n", current_pos->x, current_pos->y);
        printf("NORTH: ");
        if (current_pos->north == NULL) printf("NULL\n");
        else printf("(%d, %d)\n", current_pos->north->x, current_pos->north->y);
        printf("SOUTH: ");
        if (current_pos->south == NULL) printf("NULL\n");
        else printf("(%d, %d)\n", current_pos->south->x, current_pos->south->y);
        printf("WEST: ");
        if (current_pos->west == NULL) printf("NULL\n");
        else printf("(%d, %d)\n", current_pos->west->x, current_pos->west->y);
        printf("EAST: ");
        if (current_pos->east == NULL) printf("NULL\n\n");
        else printf("(%d, %d)\n\n", current_pos->east->x, current_pos->east->y);
        
        
        direction_to_move = SOUTH;
        num_free_paths = 0;
        pause(100);
        
        analyseSquare(current_pos, NORTH, ping_cm(8));
        turn(-90);
        pause(100);
        
        analyseSquare(current_pos, WEST, ping_cm(8));
        turn(180);
        pause(100);
        
        analyseSquare(current_pos, EAST, ping_cm(8));
        pause(100);
        
        if (num_free_paths == 1) { // if square has only one free path, it is a corridor
            current_pos->visited = CORRIDOR;
            printf("Recorded current square as a corridor\n");
        }
        
        // turn to face appropriate square
        switch (direction_to_move) {
            case NORTH:
                turn(-90);
                break;
            case WEST:
                turn(-180);
                break;
            case SOUTH:
                turn(90);
                break;
        }
        currentDirection = calculateCardinalDirection(direction_to_move);
        Square* temp = current_pos;
        
        switch (currentDirection) {
            case NORTH:
                current_pos = current_pos->north;
                current_pos->south = temp;
                break;
            case WEST:
                current_pos = current_pos->west;
                current_pos->east = temp;
                break;
            case EAST:
                current_pos = current_pos->east;
                current_pos->west = temp;
                break;
            case SOUTH:
                current_pos = current_pos->south;
                current_pos->north = temp;
            
        }
        if (current_pos->visited > TWICE) current_pos->visited = ONCE;
        
        forwards(FWD);
    }
     
}
