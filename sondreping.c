#include "simpletools.h"
#include "ping.h"

#define true 1
#define false 0

#define MAX_SPEED 128 // in ticks / sec

typedef struct Node {
    int rotation;
    int forward_movement;
    struct Node* next;
} Node;

Node* addToTop(int rot, int fwd, Node* current_node) {
    Node* newNode = malloc(sizeof(Node));
    newNode->rotation = rot;
    newNode->forward_movement = fwd;
    newNode->next = current_node;
}

Node* pop(Node* current_node)
{
    Node* node_to_pop = current_node;
}

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

Node* turnAndStore(int degrees, Node* current_node) {
    Node* new_node = addToTop(degrees, 20, current_node);
    turn(degrees);
    return new_node;
}

void forwards(int distance) {
    drive_goto(calculateTicks(distance), calculateTicks(distance));
}

void printList(Node* top_node) {
    int number = 1;
    while (top_node != NULL) {
        printf("Node #%d: Rotation: %d, Forward movement: %d.\n", number++,
        top_node->rotation, top_node->forward_movement);
        top_node = top_node->next;
    }
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

  Node* current_node = malloc(sizeof(Node));
  current_node->rotation = 0;
  current_node->forward_movement = 0;
  current_node->next = NULL;
  
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

    current_node = turnAndStore(Pout, current_node);
    forwards(20);
    
    distance = ping_cm(8);
    if (distance <= 10) {
        high(26);
        pause(100);
        low(26);
        break;
    }
    printList(current_node);   
  }
  
    turn(178);
  
    while (current_node != NULL) {
        forwards(20);
        turn(current_node->rotation * -1);
        current_node = current_node->next;
        printList(current_node);
    }
}
