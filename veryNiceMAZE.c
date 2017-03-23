
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

int prev_direction;

int isRightDirection(Square* square) {
    if (square != NULL) {
        return square->visited == ONCE;
    } else {
        return false;
    }
}

Square map = Square[4][4];

//////////////////////////////////////////////


#define MAX 16

int intArray[MAX];
int front = 0;
int rear = -1;
int itemCount = 0;

int peek() {
   return intArray[front];
}

bool isEmpty() {
   return itemCount == 0;
}

bool isFull() {
   return itemCount == MAX;
}

int size() {
   return itemCount;
}  

void insert(int data) {

   if(!isFull()) {
	
      if(rear == MAX-1) {
         rear = -1;            
      }       

      intArray[++rear] = data;
      itemCount++;
   }
}

int remove() {
   int data = intArray[front++];
	
   if(front == MAX) {
      front = 0;
   }
	
   itemCount--;
   return data;  
}


int initial = 1;
int waiting = 2;
int visited = 3;
int index = 0;

int state[16];
int adjacencyMatrix[4][4];
void populate_adjacency_matrix(int x, int y, Square* startPosition) {
    if (x >= 4 || x < 0) {
        return;
    }
    if (startPosition->north != NULL) {
        adjacencyMatrix[x][y + 1] = true; 
        populate_adjacency_matrix(x, y + 1, startPosition->north);
    }
    if (startPosition->south != NULL) {
        adjacencyMatrix[x][y - 1] = true; 
        populate_adjacency_matrix(x, y - 1, startPosition->south);
    }
    if (startPosition->east != NULL) {
        adjacencyMatrix[x + 1][y] = true; 
        populate_adjacency_matrix(x + 1, y, startPosition->east);
    }
    if (startPosition->west != NULL) {
        adjacencyMatrix[x - 1][y] = true; 
        populate_adjacency_matrix(x + 1, y, startPosition->west);
    }
}
void calculateJourney(Square* position) {
    populate_adjacency_matrix(0, 0, position);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%d ", adjacencyMatrix[i][j]);
        }
        printf("------");
    }
    int from = south;

    if (from != north && start->north != NULL) insert(north);
    if (from != east && start->east != NULL) insert(east);
    if (from != west && start->west != NULL) insert(west);
    index = 2;
    
    int current;
    while (!isEmpty()) {
        current = remove();
        printf("%d", current);

        if (from != north && position->north) {
            insert(north);
        }
        if (from != east && position->east) {
            insert(east);
        }
        if (from != west && position->west) {
            insert(west);
        }
        if (from != south && position->south) {
            insert(south);
        }
         if (links[i] != NULL) {
            switch(i) {
                case north:
                    links[i] = NULL;
                    checkSquare(position->north);
                    break;
                case east:
                    if (position->east->north != NULL) {
                        links[i] = north;
                    }
                    break;
                case south:
                if (position->south->south != NULL) {
                    links[i] = north;
                }
                break;
                case west:
                if (position->north->north != NULL) {
                    links[i] = north;
                }
                break;
            }
        }
        if (from != north && start->north != NULL) {
            links[index++] = north;
        }
        if (from != east && start->east != NULL) {
            links[index++] = east;
        }
        if (from != south && start->south != NULL) {
            links[index++] = south;
        }
        if (from != west && start->west != NULL) {
            links[index++] = west;
        }
        loopThroughLinks();
    }

}

////////////////////////////////////////////////

void returnJourney() {
    prev_direction = currentDirection;
    while (true) {
        if (current_pos->x == 0 && current_pos->y == 0) {
            high(26);
            printf("Reached the beginning again\n");
            pause(200);
            low(26);
            break;
        }
        if (ping_cm(8) < 15) forwards(-20);
        if (isRightDirection(current_pos->south) && prev_direction != NORTH) {
            printf("GOING SOUTH\n");
            current_pos = current_pos->south;
            switch (currentDirection) {
                case NORTH:
                    turn(180);
                    break;
                case WEST:
                    turn(-90);
                    break;
                case EAST:
                    turn(90);
                    break;
            }
            currentDirection = SOUTH;
            forwards(FWD);
        } else if (isRightDirection(current_pos->west) && prev_direction != EAST) {
            printf("GOING WEST\n");
            current_pos = current_pos->west;
            switch (currentDirection) {
                case NORTH:
                    turn(-90);
                    break;
                case SOUTH:
                    turn(90);
                    break;
                case EAST:
                    turn(180);
                    break;
            }
            currentDirection = WEST;
            forwards(FWD);
        } else if (isRightDirection(current_pos->north) && prev_direction != SOUTH) {
            printf("GOING NORTH\n");
            current_pos = current_pos->north;
            switch (currentDirection) {
                case SOUTH:
                    turn(180);
                    break;
                case WEST:
                    turn(90);
                    break;
                case EAST:
                    turn(-90);
                    break;
            }
            currentDirection = NORTH;
            forwards(FWD);
        } else if (isRightDirection(current_pos->east) && prev_direction != WEST) {
            printf("GOING EAST\n");
            current_pos = current_pos->east;
            switch (currentDirection) {
                case NORTH:
                    turn(90);
                    break;
                case WEST:
                    turn(180);
                    break;
                case SOUTH:
                    turn(-90);
                    break;
            }
            currentDirection = EAST;
            forwards(FWD);
        }
        prev_direction = currentDirection;
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
                return;
            }
            else if (current_pos->north->visited == UNKNOWN || current_pos->north->visited == NEVER) {
                current_pos->north->visited = NEVER;
                lowest_visited_option = NEVER;
                direction_to_move = localDirection;
                printf("Found unvisited square to the north\n");
            }
            else {
                /*if (current_pos->visited == NEVER && current_pos->south->visited <= lowest_visited_option) {
                    lowest_visited_option = current_pos->south->visited;
                    direction_to_move = localDirection;
                }*/
                if (current_pos->north->visited < lowest_visited_option) {
                    lowest_visited_option = current_pos->north->visited;
                    direction_to_move = localDirection;
                }
                printf("Found previously visited square to the north\n");
            }
            num_free_paths++;    
            return;
        case WEST:
            if (wall_distance < 40 || current_pos->west == NULL) {
                current_pos->west = NULL;
                printf("Detected wall to the west\n");
                return;
            }
            if (current_pos->west->visited == UNKNOWN || current_pos->west->visited == NEVER) {
                current_pos->west->visited = NEVER;
                lowest_visited_option = NEVER;
                direction_to_move = localDirection;
                printf("Found unvisited square to the west\n");
            }
            else {
                /*if (current_pos->visited == NEVER && current_pos->south->visited <= lowest_visited_option) {
                    lowest_visited_option = current_pos->south->visited;
                    direction_to_move = localDirection;
                }*/
                if (current_pos->west->visited < lowest_visited_option) {
                    lowest_visited_option = current_pos->west->visited;
                    direction_to_move = localDirection;
                }
                printf("Found previously visited square to the west\n");
            }
            num_free_paths++;    
            return;
        case EAST:
            if (wall_distance < 40 || current_pos->east == NULL) {
                current_pos->east = NULL;
                printf("Detected wall to the east\n");
                return;
            }
            else if (current_pos->east->visited == UNKNOWN || current_pos->east->visited == NEVER) {
                current_pos->east->visited = NEVER;
                lowest_visited_option = NEVER;
                direction_to_move = localDirection;
                printf("Found unvisited square to the east\n");
            }
            else {
                /*if (current_pos->visited == NEVER && current_pos->south->visited <= lowest_visited_option) {
                    lowest_visited_option = current_pos->south->visited;
                    direction_to_move = localDirection;
                }*/
                if (current_pos->east->visited < lowest_visited_option) {
                    lowest_visited_option = current_pos->east->visited;
                    direction_to_move = localDirection;
                }
                printf("Found previously visited square to the east\n");
            }
            num_free_paths++;    
            return;
        case SOUTH:
            if (wall_distance < 40 || current_pos->south == NULL) {
                current_pos->south = NULL;
                printf("Detected wall to the south\n");
                return;
            }
            else if (current_pos->south->visited == UNKNOWN || current_pos->south->visited == NEVER) {
                current_pos->south->visited = NEVER;
                lowest_visited_option = NEVER;
                direction_to_move = localDirection;
                printf("Found unvisited square to the south\n");
            }
            else {
                /*if (current_pos->visited == NEVER && current_pos->south->visited <= lowest_visited_option) {
                    lowest_visited_option = current_pos->south->visited;
                    direction_to_move = localDirection;
                }*/
                if (current_pos->south->visited < lowest_visited_option) {
                    lowest_visited_option = current_pos->south->visited;
                    direction_to_move = localDirection;
                }
                printf("Found previously visited square to the south\n");
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
            turn(-90);
            pause(100);
            
            analyseSquare(current_pos, WEST, ping_cm(8));
            turn(180);
            pause(100);
            
            analyseSquare(current_pos, EAST, ping_cm(8));
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
                turn_amount = (turned) ? -90 : 0;
                break;
            case WEST:
                turn_amount = (turned) ? -180 : -90;
                break;
            case SOUTH:
                turn_amount = (turned) ? 90 : 180;
                break;
            case EAST:
                turn_amount = (turned) ? 0 : 90;
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
        
        if (current_pos->x == 3 && current_pos->y == 3) {
            high(26);
            printf("Reached the end of the maze!\n");
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
    returnJourney();
}
