
#include "simpletools.h"
#include "ping.h"
#include "abdrive.h"
#include <stdio.h>

#define true 1
#define false 0

#define UNKNOWN 0
#define NEVER 1
#define ONCE 2
#define TWICE 3
#define CORRIDOR 4

#define NORTH 0
#define WEST 1
#define EAST 2
#define SOUTH 3

// Queue and BFS:
const int initial = 1;
const int waiting = 2;
const int visited = 3;
#define MAX 16

#define MAX_SPEED 128 // in ticks / sec
#define FWD 420

int currentDirection;

typedef struct Square {
    int x;
    int y;
    int visited;
    int is_junction;
    struct Square* north;
    struct Square* west;
    struct Square* east;
    struct Square* south;
} Square;

double radius = 52.9;

// BFS:
int state[MAX];
int adjacencyMatrix[MAX][MAX];

// QUEUE:
int queue[MAX];
int front = 0;
int rear = -1;
int itemCount = 0;

Square* grid[4][4];

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
    } else if (degrees == -180) {
        drive_goto(-50, 52);
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



int isEmpty() {
   return itemCount == 0;
}

int isFull() {
   return itemCount == MAX;
} 

void insert(int data) {

   if(!isFull()) {
	
      if(rear == MAX-1) {
         rear = -1;            
      }       

      queue[++rear] = data;
      itemCount++;
   }
}

int pop() {
   int data = queue[front++];
	
   if(front == MAX) {
      front = 0;
   }
	
   itemCount--;
   return data;  
}

int convertMovementToDirection(int square1, int square2) {
    if (square1 - 4 == square2) {
        return SOUTH;
    } else if (square1 + 4 == square2) {
        return NORTH;
    } else if (square1 - 1 == square2) {
        return WEST;
    } else if (square1 + 1 == square2) {
        return EAST;
    }
}

int * calculateJourney(int start, int end) {
    Square* position = grid[0][0];
    printf("calculating froge...\n");

    for(int v = 0; v < 16; v++) { // initialise state to initial
        state[v] = initial;
    }

    int from = SOUTH;
    insert(start);
    state[start] = waiting;
    int current, i;
    int previous[20];

    while (!isEmpty()) {
        current = pop();
        printf("Current: %d \n", current);
        state[current] = visited;

        for(i = 0; i < 16; i++) {
            if(adjacencyMatrix[current][i] == true && state[i] == initial) {
                insert(i);
                state[i] = waiting;
                previous[i] = current;
                if (i == end) {
                    printf("FOUND! printing: \n");
                    int directions[20];
                    int index = 0;
                    int p = end;
                    int lengthOfDirections = 0;
                    directions[index++] = p;
                    while ((p = previous[p]) != 0) {
                        directions[index++] = p;
                        lengthOfDirections++;
                    }
                    lengthOfDirections += 2;
                    directions[index] = start;
                    int *retArray = malloc(lengthOfDirections);
                    for (int i = 0; i < lengthOfDirections; i++) {
                        retArray[i] = directions[index--];
                        
                        printf("%d\n", retArray[i]);
                    }
                    // Convert list of squares to list of directions
                    index = 0;
                    for (int j = 0; j < lengthOfDirections; j+=2) {
                        printf("direction : %d \n", convertMovementToDirection(retArray[j], retArray[j+1]));
                        retArray[index++] = convertMovementToDirection(retArray[j], retArray[j+1]);
                    }
                    retArray[index] = 69; // stop when you reach 69
                    return retArray;
                }
            }
        }
    }
}

int convertXYToSquareNumber(int x, int y) {
    switch (y) {
        case 0:
            return 0 + x;
        case 1:
            return 4 + x;
        case 2:
            return 8 + x;
        case 3:
            return 12 + x;
    }
    return 69;
}

void add_to_adjacency_matrix(int x, int y, int direction, int boolean) {
    int x_new, y_new;
    switch(direction) {
        case NORTH:
            if (y > 2) return;
            x_new = x;
            y_new = y + 1;
            break;
        case SOUTH:
            if (y < 1) return;
            x_new = x;
            y_new = y - 1;
        case WEST:
            if (x < 1) return;
            x_new = x - 1;
            y_new = y;
        case EAST:
            if (x > 2) return;
            x_new = x + 1;
            y_new = y;
            break;
    }
    printf("adj[%d][%d] = %d \n",convertXYToSquareNumber(x, y),convertXYToSquareNumber(x_new, y_new),boolean);
    adjacencyMatrix[convertXYToSquareNumber(x, y)][convertXYToSquareNumber(x_new, y_new)] = boolean;

}

int * convertSquareNumberToXY(int square) {
    static int ret[2];
    if (square > 11) {
        ret[0] = 0;
        ret[1] = square - 12;
        return ret;
    }
    if (square > 7) {
        ret[0] = 1;
        ret[1] = square - 8;
        return ret;
    }
    if (square > 3) {
        ret[0] = 2;
        ret[1] = square - 4;
        return ret;
    }
    ret[0] = 3;
    ret[1] = square - 1;
    return ret;
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

void turnGlobal(int direction_to_turn) {
    switch (direction_to_turn) {
        case SOUTH:
            printf("GOING SOUTH\n");
            switch (currentDirection) {
                case NORTH:
                    turn(180);
                    return;
                case WEST:
                    turn(-90);
                    return;
                case EAST:
                    turn(90);
                    return;
            }
            break;
        case WEST:
            printf("GOING WEST\n");
            switch (currentDirection) {
                case NORTH:
                    turn(-90);
                    return;
                case SOUTH:
                    turn(90);
                    return;
                case EAST:
                    turn(180);
                    return;
            }
            break;
        case NORTH:
            printf("GOING NORTH\n");
            switch (currentDirection) {
                case SOUTH:
                    turn(180);
                    return;
                case WEST:
                    turn(90);
                    return;
                case EAST:
                    turn(-90);
                    return;
            }
            break;
        case EAST:
            printf("GOING EAST\n");
            switch (currentDirection) {
                case NORTH:
                    turn(90);
                    return;
                case WEST:
                    turn(180);
                    return;
                case SOUTH:
                    turn(-90);
                    return;
            }
            break;
    }
}

int isRightDirection(Square* square) {
    if (square != NULL) {
        return square->visited == ONCE || square->is_junction;
    } else {
        return false;
    }
}

void returnJourney() {
    while (true) {
        if (current_pos->x == 0 && current_pos->y == 0) {
            high(26);
            printf("Reached the beginning again\n");
            pause(200);
            low(26);
            break;
        }
        if (ping_cm(8) < 15) forwards(-20);
        if (isRightDirection(current_pos->south) && currentDirection != NORTH) {
            current_pos = current_pos->south;
            turnGlobal(SOUTH);
            currentDirection = SOUTH;
            forwards(FWD);
        } else if (isRightDirection(current_pos->west) && currentDirection != EAST) {
            current_pos = current_pos->west;
            turnGlobal(WEST);
            currentDirection = WEST;
            forwards(FWD);
        } else if (isRightDirection(current_pos->north) && currentDirection != SOUTH) {
            current_pos = current_pos->north;
            turnGlobal(NORTH);
            currentDirection = NORTH;
            forwards(FWD);
        } else if (isRightDirection(current_pos->east) && currentDirection != WEST) {
            current_pos = current_pos->east;
            turnGlobal(EAST);
            currentDirection = EAST;
            forwards(FWD);
        }
        else {
            printf("Going nowhere... \n");
            printf("Current direction is %d, pos is %d, %d\n", currentDirection, current_pos->x, current_pos->y);
            printf("Surrounding squares are \n");
            if (current_pos->north == NULL) printf("North: NULL\n");
            else printf("North: open path\n");
            if (current_pos->west == NULL) printf("West: NULL\n");
            else printf("West: open path\n");
            if (current_pos->east == NULL) printf("East: NULL\n");
            else printf("East: open path\n");
            if (current_pos->south == NULL) printf("South: NULL\n");
            else printf("South: open path\n");
            
            break;
        }
    }
}

void navigateToEnd()
{
    
}

void phaseTwo(int* way_back) {
    currentDirection = NORTH;
    int forward_sum = 0;
    int global_direction;
    for (int i = 0; way_back[i] != 69; i++) {
        global_direction = calculateCardinalDirection(currentDirection);
        if (way_back[i] == global_direction) {
            forward_sum += FWD;
        } else {
            forwards(forward_sum);
            turnGlobal(way_back[i]);
            forward_sum = FWD;
            currentDirection = way_back[i];
        }
    }
    forwards(forward_sum);
    high(26);
    printf("Phase two complete!\n");
    pause(200);
    low(26);
}

int direction_to_move, num_free_paths;


Square* initGrid() {
    Square* origin = malloc(sizeof(Square));
    Square* square = origin;
    grid[0][0] = origin;
    for (int i = 0; i < 4; i++) {
        Square* leftmost = square;
        for (int j = 0; j < 4; j++) {
            square->x = j;
            square->y = i;
            square->visited = UNKNOWN;
            square->is_junction = false;
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

int lowest_visited_option, num_free_paths;
Square* prev_square;

void analyseSquare(Square* current_pos, int localDirection, int wall_distance) {
    if (wall_distance < 10) {
        forwards(-30); // back off a little if wall is too close
    }
    int globalDirection = calculateCardinalDirection(localDirection);
    switch (globalDirection) {
        case NORTH:
            if (wall_distance < 40 || current_pos->north == NULL) {
                current_pos->north = NULL;
                printf("Detected wall to the north\n");
                add_to_adjacency_matrix(current_pos->x, current_pos->y, NORTH, 0);
                return;
            }
            else if (current_pos->north->visited == UNKNOWN || current_pos->north->visited == NEVER) {
                current_pos->north->visited = NEVER;
                lowest_visited_option = NEVER;
                direction_to_move = localDirection;
                printf("Found unvisited square to the north\n");
                add_to_adjacency_matrix(current_pos->x, current_pos->y, NORTH, 1);
            }
            else {
                if (current_pos->north->visited < lowest_visited_option) {
                    lowest_visited_option = current_pos->north->visited;
                    direction_to_move = localDirection;
                }
                printf("Found previously visited square to the north\n");
                add_to_adjacency_matrix(current_pos->x, current_pos->y, NORTH, 1);
            }
            num_free_paths++;    
            return;
        case WEST:
            if (wall_distance < 40 || current_pos->west == NULL) {
                current_pos->west = NULL;
                printf("Detected wall to the west\n");
                add_to_adjacency_matrix(current_pos->x, current_pos->y, WEST, 0);
                return;
            }
            if (current_pos->west->visited == UNKNOWN || current_pos->west->visited == NEVER) {
                current_pos->west->visited = NEVER;
                lowest_visited_option = NEVER;
                direction_to_move = localDirection;
                printf("Found unvisited square to the west\n");
                add_to_adjacency_matrix(current_pos->x, current_pos->y, WEST, 1);
            }
            else {
                if (current_pos->west->visited < lowest_visited_option) {
                    lowest_visited_option = current_pos->west->visited;
                    direction_to_move = localDirection;
                }
                printf("Found previously visited square to the west\n");
                add_to_adjacency_matrix(current_pos->x, current_pos->y, WEST, 1);
            }
            num_free_paths++;    
            return;
        case EAST:
            if (wall_distance < 40 || current_pos->east == NULL) {
                current_pos->east = NULL;
                printf("Detected wall to the east\n");
                add_to_adjacency_matrix(current_pos->x, current_pos->y, EAST, 0);
                return;
            }
            else if (current_pos->east->visited == UNKNOWN || current_pos->east->visited == NEVER) {
                current_pos->east->visited = NEVER;
                lowest_visited_option = NEVER;
                direction_to_move = localDirection;
                printf("Found unvisited square to the east\n");
                add_to_adjacency_matrix(current_pos->x, current_pos->y, EAST, 1);
            }
            else {
                if (current_pos->east->visited < lowest_visited_option) {
                    lowest_visited_option = current_pos->east->visited;
                    direction_to_move = localDirection;
                }
                printf("Found previously visited square to the east\n");
                add_to_adjacency_matrix(current_pos->x, current_pos->y, EAST, 1);
            }
            num_free_paths++;    
            return;
        case SOUTH:
            if (wall_distance < 40 || current_pos->south == NULL) {
                current_pos->south = NULL;
                printf("Detected wall to the south\n");
                add_to_adjacency_matrix(current_pos->x, current_pos->y, SOUTH, 0);
                return;
            }
            else if (current_pos->south->visited == UNKNOWN || current_pos->south->visited == NEVER) {
                current_pos->south->visited = NEVER;
                lowest_visited_option = NEVER;
                direction_to_move = localDirection;
                printf("Found unvisited square to the south\n");
                add_to_adjacency_matrix(current_pos->x, current_pos->y, SOUTH, 1);
            }
            else {
                if (current_pos->south->visited < lowest_visited_option) {
                    lowest_visited_option = current_pos->south->visited;
                    direction_to_move = localDirection;
                }
                printf("Found previously visited square to the south\n");
                add_to_adjacency_matrix(current_pos->x, current_pos->y, SOUTH, 1);
            }
            num_free_paths++;    
            return;
    }
}

int continuePath(Square* current_pos) {
    int way_back = calculateCardinalDirection(SOUTH);
    for (int i = NORTH; i < SOUTH; i++) {
        int globalDirection = calculateCardinalDirection(i);
        switch (globalDirection) {
            case NORTH:
                if (current_pos->north != NULL && way_back != NORTH) return i;
                break;
            case WEST:
                if (current_pos->west != NULL && way_back != WEST) return i;
                break;
            case EAST:
                if (current_pos->east != NULL && way_back != EAST) return i;
                break;
            case SOUTH:
                if (current_pos->south != NULL && way_back != SOUTH) return i;
                break;
        }
    }
    return SOUTH;
}

int main() {
    low(26);  
    low(27);
    forwards(FWD);
    start_pos = initGrid();
    start_pos->visited = NEVER;
    start_pos->south = NULL;
    current_pos = start_pos;
    currentDirection = NORTH;
    int temp_direction;
    int turned;
    int turn_amount;
        
    while (true) {
        printf("\nEntered square (%d, %d)\n", current_pos->x, current_pos->y);        
        direction_to_move = SOUTH;
        turned = false;
        num_free_paths = 0;
        pause(100);
        
        if (current_pos->visited < ONCE) {
            turned = true;
        
            analyseSquare(current_pos, NORTH, ping_cm(8));
            turn(90);
            pause(100);
            
            analyseSquare(current_pos, EAST, ping_cm(8));
            turn(-180);
            pause(100);
            
            analyseSquare(current_pos, WEST, ping_cm(8));
            pause(100);
            
            if (num_free_paths > 1) { 
                current_pos->is_junction = true;
                printf("Recorded current square as a junction\n");
            }
        }    
        else {
            if (!current_pos->is_junction) { // if square is part of a path, just keep going
                printf("Continuing down previously visited path...\n");
                direction_to_move = continuePath(current_pos);
            }
            else {
                analyseSquare(current_pos, NORTH, 69);
                analyseSquare(current_pos, WEST, 69);
                analyseSquare(current_pos, EAST, 69);
            }
        }
        // turn to face appropriate square
        switch (direction_to_move) {
            case NORTH:
                turn_amount = (turned) ? 90 : 0;
                break;
            case WEST:
                turn_amount = (turned) ? 0 : -90;
                break;
            case SOUTH:
                turn_amount = (turned) ? -90 : 180;
                break;
            case EAST:
                turn_amount = (turned) ? 180 : 90;
                break;
        }
        turn(turn_amount);
        
        if (current_pos->visited < TWICE) {
            if (prev_square != NULL) {
                if (prev_square->is_junction) {
                    current_pos->visited++;
                }
                else {
                    current_pos->visited = ONCE;
                }
            }
            else current_pos->visited = ONCE;    
        } 
            
        currentDirection = calculateCardinalDirection(direction_to_move);
        prev_square = current_pos;
        
        if (grid[0][3]->visited > NEVER && grid[3][0]->visited > NEVER && grid[3][3]->visited > NEVER) {
            high(26);
            printf("Visited all corners of the maze!\n");
            if (current_pos->x == 3 && current_pos->y == 3) {
                break;
            } else {
                navigateToEnd();
            }
            pause(200);
            low(26);
            break;
        }
        
        switch (currentDirection) {
            case NORTH:
                current_pos = current_pos->north;
                break;
            case WEST:
                current_pos = current_pos->west;
                break;
            case EAST:
                current_pos = current_pos->east;
                break;
            case SOUTH:
                current_pos = current_pos->south;
                break;
        }
        
        if (current_pos->is_junction && prev_square->visited < TWICE) prev_square->visited++;
        lowest_visited_option = prev_square->visited;
        
        forwards(FWD);
    }
    //returnJourney();
    calculateJourney(0, 15);
}
