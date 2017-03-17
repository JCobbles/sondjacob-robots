
#include "simpletools.h"
#include "ping.h"
#include "abdrive.h"

#define true 1
#define false 0

#define NEVER 0
#define ONCE 1
#define TWICE 2
#define CORRIDOR 3

#define NO_UNVISITED_PATH 0
#define NORTH 0
#define WEST 3
#define EAST 1
#define SOUTH 2

#define MAX_SPEED 128 // in ticks / sec
#define FWD 40

typedef struct Square {
    int x : 3;
    int y : 3;
    int visited : 2;
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
    double radians = degrees *  PI / 180;
    int ticks = calculateTicks(radians * radius);
    drive_goto(ticks, -ticks);
}

void forwards(int distance) {
    drive_goto(calculateTicks(distance), calculateTicks(distance));
}


int reverseDirection(int direction) {
    return (direction - 2) % 4;
}

int calculateCardinalDirection(int localDirection) {
    if (localDirection == FORWARDS) return currentDirection;
    if (localDirection == LEFT) return --currentDirection;
    return ++currentDirection;
}

int isRightDirection(Square* square) {
    return square->visited == ONCE || square->visited == PATH;
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
    while (1) {
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

Square* current_pos;


int main() {
    int distance, irLeft, irRight;
    int direction_to_move;
    low(26);  
    low(27);
    forwards(FWD);
    current_pos = calloc(1, sizeof(Square));
    current_pos->x = 0;
    current_pos->y = 0;
    current_pos->visited = NEVER;
        
    while (true) {
        direction_to_move = NO_UNVISITED_PATH;
        
        for(int dacVal = 0; dacVal < 160; dacVal += 8) {                                               
            dac_ctr(26, 0, dacVal);   
            freqout(11, 1, 38000);      
            irLeft += input(10); 

            dac_ctr(27, 1, dacVal);
            freqout(1, 1, 38000);
            irRight += input(2);                  
        }
        
        if (irLeft < 20) {
            // TODO: convert this to a global position
            // mark that there is a wall to the left
            current_pos->west = NULL;
        } else if (current_pos->west == NULL) {
            // if square to left has not been seen, allocate a position for it in memory
            current_pos->west = calloc(1, sizeof(Square));
            current_pos->west->x = current_pos->x - 1;
            current_pos->west->y = current_pos->y;
            current_pos->west->visited = NEVER;
            direction_to_move = WEST;
        }
        else if (current_pos->west->visited == NEVER) {
            direction_to_move = WEST;
        }
        
        if (irRight < 20) {
            // TODO: convert this to a global position
            current_pos->east = NULL;
        } else if (current_pos->east == NULL) {
            current_pos->east = calloc(1, sizeof(Square));
            current_pos->east->x = current_pos->x + 1;
            current_pos->east->y = current_pos->y;
            current_pos->east->visited = NEVER;
            if (direction_to_move == NO_UNVISITED_PATH) {
                direction_to_move = EAST;
            }
        } else if (current_pos->east->visited == NEVER && direction_to_move == NO_UNVISITED_PATH) {
            direction_to_move = EAST;
        }
        
        distance = ping_cm(8);
        if (distance < 20) {
            // TODO: convert this to a global position
            current_pos->north = NULL;
        } else if (current_pos->north == NULL) {
            current_pos->north->x = current_pos->x;
            current_pos->north->y = current_pos->y + 1;
            current_pos->north->visited = NEVER;
            if (direction_to_move == NO_UNVISITED_PATH) {
                direction_to_move = NORTH;
            }
        } else if (current_pos->north->visited == NEVER && direction_to_move == NO_UNVISITED_PATH) {
            direction_to_move = NORTH;
        }
        
        switch (direction_to_move) {
            Square* temp = current_pos;
            case NORTH:
                forwards(FWD);
                current_pos = current_pos->north;
                current_pos->south = temp;
                current_pos->visited++;
                break;
            case WEST:
                turn(90);
                forwards(FWD);
                current_pos = current_pos->west;
                current_pos->east = temp;
                current_pos->visited++;
                break;
            case EAST:
                turn(-90);
                forwards(FWD);
                current_pos = current_pos->east;
                current_pos->west = temp;
                current_pos->visited++;
                break;
            case NO_UNVISITED_PATH:
                turn(180);
                forwards(FWD);
                break;
        }
    }
    printf("Iteration complete\n");
}

