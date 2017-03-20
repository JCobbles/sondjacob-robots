
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
#define UNKNOWN 4

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

Square* current_pos;
Square* start_pos;

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

Square* initGrid() {
    Square* origin = malloc(sizeof(Square));
    Square* square = origin;
    Square* grid[4][4];
    grid[0][0] = origin;
    for (int i = 0; i < 4; i++) {
        Square* leftmost = square;
        for (int j = 0; j < 4; j++) {
            square->x = j;
            square->y = i;
            square->visited = UNKNOWN;
            grid[j][i] = square;
            if (i != 0) {
                square->south = grid[j][i - 1];
                grid[j][i - 1]->north = square;
            }
            if (j != 3) {
                Square* temp = square;
                square->east = malloc(sizeof(Square));
                square = square->east;
                square->west = temp;
            }
        }
        leftmost->north = malloc(sizeof(Square));
        square = leftmost->north;
    }
    return origin;
}

int found_new_path, prev_square_visited;

void analyseSquare(Square* current_pos, int localDirection, int wall_distance) {
    if (wall_distance < 10) {
        forwards(-20); // back off a little if wall is too close
    }
    int globalDirection = calculateCardinalDirection(localDirection);
    switch (globalDirection) {
        case NORTH:
            if (wall_distance < 40) {
                current_pos->north = NULL;
                printf("Detected wall to the north\n");
                return;
            }
            if (current_pos->north->visited == UNKNOWN) {
                current_pos->north->visited = NEVER;
                direction_to_move = localDirection;
                printf("Recorded unvisited square to the north\n");
            }
            else if (current_pos->north->visited == NEVER) {
                direction_to_move = localDirection;
                printf("Found unvisited square to the north\n");
            }
            else {
                if (!found_new_path && prev_square_visited >= current_pos->north->visited) {
                    direction_to_move = localDirection;
                    found_new_path = true;
                } 
                printf("Found previously visited square to the north\n");
                return;
            }
            found_new_path = true;
            return;
        case WEST:
            if (wall_distance < 40) {
                current_pos->west= NULL;
                printf("Detected wall to the west\n");
                return;
            }
            if (current_pos->west->visited == UNKNOWN) {
                current_pos->west->visited = NEVER;
                direction_to_move = localDirection;
                printf("Recorded unvisited square to the west\n");
            }
            else if (current_pos->west->visited == NEVER) {
                direction_to_move = localDirection;
                printf("Found unvisited square to the west\n");
            }
           else {
                if (!found_new_path && prev_square_visited >= current_pos->west->visited) {
                    direction_to_move = localDirection;
                    found_new_path = true;
                } 
                printf("Found previously visited square to the west\n");
                return;
            }
            found_new_path = true;
            return;
        case EAST:
            if (wall_distance < 40) {
                current_pos->east= NULL;
                printf("Detected wall to the east\n");
                return;
            }
            if (current_pos->east->visited == UNKNOWN) {
                current_pos->east->visited = NEVER;
                direction_to_move = localDirection;
                printf("Recorded unvisited square to the east\n");
            }
            else if (current_pos->east->visited == NEVER) {
                direction_to_move = localDirection;
                printf("Found unvisited square to the east\n");
            }
            else {
                if (!found_new_path && prev_square_visited >= current_pos->east->visited) {
                    direction_to_move = localDirection;
                    found_new_path = true;
                } 
                printf("Found previously visited square to the east\n");
                return;
            }
            found_new_path = true;
            return;
        case SOUTH:
            if (current_pos->x == 0 && current_pos->y == 0) {
                printf("Found starting position to the south. Probably don't want to go there lol\n");
                return;
            }
            if (wall_distance < 40) {
                current_pos->south = NULL;
                printf("Detected wall to the south\n");
                return;
            }
            if (current_pos->south->visited == UNKNOWN) {
                current_pos->south->visited = NEVER;
                direction_to_move = localDirection;
                printf("Recorded unvisited square to the south\n");
            }
            else if (current_pos->south->visited == NEVER) {
                direction_to_move = localDirection;
                printf("Found unvisited square to the south\n");
            }
            else {
                if (!found_new_path && prev_square_visited >= current_pos->south->visited) {
                    direction_to_move = localDirection;
                    found_new_path = true;
                } 
                printf("Found previously visited square to the south\n");
                return;
            }
            found_new_path = true;
            return;
    }
}

int main() {
    low(26);  
    low(27);
    forwards(FWD);
    start_pos = initGrid();
    start_pos->visited = ONCE;
    current_pos = start_pos;
    currentDirection = NORTH;    
        
    while (true) {
        printf("\nEntered square (%d, %d)\n", current_pos->x, current_pos->y);        
        direction_to_move = SOUTH;
        found_new_path = false;
        pause(100);
        
        analyseSquare(current_pos, NORTH, ping_cm(8));
        turn(-90);
        pause(100);
        
        analyseSquare(current_pos, WEST, ping_cm(8));
        turn(180);
        pause(100);
        
        analyseSquare(current_pos, EAST, ping_cm(8));
        pause(100);
        
        /* if (num_free_paths == 1) { // if square has only one free path, it is a corridor
            current_pos->visited = CORRIDOR;
            printf("Recorded current square as a corridor\n");
        } */
        
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
        prev_square_visited = current_pos->visited;
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
        if (current_pos->visited < TWICE) current_pos->visited++;
        
        forwards(FWD);
    }
     
}
