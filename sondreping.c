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

int calculateTicks(int distanceInMillimetres) {
  return newRound(distanceInMillimetres / 3.25);
}

int turn(int degrees)
{
  double radians = degrees *  PI / 180;
  int ticks = calculateTicks(radians * radius);
  drive_goto(ticks, -ticks);
}

void forwards(int distance) {
    drive_goto(calculateTicks(distance), calculateTicks(distance));
}

int irLeft, irRight;
double Kp = 1;

double calculatePout(int left, int right) {
  return Kp * (right - left);
}

int main() {
  drive_speed(MAX_SPEED, MAX_SPEED);
  int distance, irLeft, irRight;

  low(26);  
  low(27);  

  while(1) {
    irLeft = 0;                                     
    irRight = 0;                                    

    for(int dacVal = 0; dacVal < 160; dacVal += 8) {                                               
      dac_ctr(26, 0, dacVal);   
      freqout(11, 1, 38000);      
      irLeft += input(10); 

      dac_ctr(27, 1, dacVal);
      freqout(1, 1, 38000);
      irRight += input(2);                  
    }                                               

    print("%c irLeft = %d, irRight = %d%c \n", 
    HOME,   irLeft, irRight, CLREOL);

    double Pout = calculatePout(irLeft, irRight);

    turn(Pout);
    forwards(20);
    distance = ping_cm(8);
    if (distance <= 10) {
        high(26);
        pause(100);
        low(26);
        break;
    }
    
    pause(10);
  }
}
