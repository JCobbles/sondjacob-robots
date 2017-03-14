
#include "simpletools.h"
#include "ping.h"
#include "abdrive.h"

#define true 1
#define false 0

#define NEVER 0
#define ONCE 1
#define TWICE 2
#define PATH 3

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

Square* current_pos;


int main() {
    int distance, irLeft, irRight;
    low(26);  
    low(27);
    forwards(FWD);
    current_pos = malloc(sizeof(Square));
    current_pos->x = 0;
    current_pos->y = 0;
        
    while (true) {
        for(int dacVal = 0; dacVal < 160; dacVal += 8) {                                               
            dac_ctr(26, 0, dacVal);   
            freqout(11, 1, 38000);      
            irLeft += input(10); 

            dac_ctr(27, 1, dacVal);
            freqout(1, 1, 38000);
            irRight += input(2);                  
        }
        
        
        
    }
    
    
}

