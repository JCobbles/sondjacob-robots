#include "simpletools.h"
#include "abdrive.h"
#include "ping.h"

const double radius = 52.9;

int newRound(double x) {
    if (x - (int) x < 0.5) {
        return (int) x;
    } else {
        return (int) x + 1;
    }
}

int calculateTicks(int distanceInMillimetres) {
    return newRound(distanceInMillimetres / 3.25);
}

void forwards(int distance) {
    drive_goto(calculateTicks(distance), calculateTicks(distance));
}
int turn(int degrees) {
    double radians = degrees *  PI / 180;
    int ticks = calculateTicks(radians * radius);
    drive_goto(ticks, -ticks);
}

int distance;


void findDirection() {

}

int main() {
    while(1) {
        distance = ping_cm(8);
        printf("distance: %dcm\n", distance);
        if (distance < 10) {
            turn(90);
        }
        findDirection();
        forwards(50);
    }
}
