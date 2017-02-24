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
double kp = 1, ki = 1, kd = 1;

double derive(double (*f)(double), double x0) {
    const double delta = 1.0e-6;
    double x1 = x0 - delta;
    double x2 = x0 + delta;
    double y1 = f(x1);
    double y2 = f(x2);
    return (y2 - y1) / (x2 - x1);
}

double integrate(int limA, int limB) {
    float i, sum = 0;
    if (limA > limB) {
        i = limA;
        limA = limB;
        limB = i;
    }

    for (i = limA; i < limB; i += (limB - limA) / N) {
        y = x * x + 2 * x - 4;
        sum += y * (limB - limA) / N;
    }
    return sum;
}

double calcError() {
    return
}


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
