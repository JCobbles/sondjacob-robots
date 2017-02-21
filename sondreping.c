#include "simpletools.h"
#include "ping.h"

#define true 1
#define false 0

#define MAX_SPEED 128 // in ticks / sec

double radius = 52.9;

int newRound(double x)
{
  if (x - (int) x < 0.5) return (int) x;
  else return (int) x + 1;
}

int calculateTicks(int distanceInMillimetres)
{
  return newRound(distanceInMillimetres / 3.25);
}

int turn(int degrees)
{
  double radians = degrees *  PI / 180;
  int ticks = calculateTicks(radians * radius);
  drive_goto(ticks, -ticks);
}

int main()
{
    int distance, irLeft, irRight;
    drive_speed(MAX_SPEED, MAX_SPEED);
    low(26);
    low(27);
    while(true)
    {
        freqout(11, 1, 38000);
        irLeft = input(10);
        
        freqout(11, 1, 38000);
        irRight = input(2);
        
        printf("irLeft: %d\n", irLeft);
        printf("irRight: %d\n", irRight);
        
        // Add main loop code here.
        distance = ping_cm(8);
        if (distance < 15)
        {
            turn(90);
        }
        printf("Distance: %d\n\n", distance);
        drive_goto(calculateTicks(100), calculateTicks(100));
    }  
}