
#include "simpletools.h"
#include "ping.h"

#define true 1
#define false 0

#define MAX_SPEED 128 // in ticks / sec
#define FWD 20

typedef struct Node {
    int left;
    int right;
    struct Node* next;
} Node;

Node* push(int ticksLeft, int ticksRight, Node* current_node) {
    Node* newNode = malloc(sizeof(Node));
    newNode->left = ticksLeft;
    newNode->right = ticksRight;
    newNode->next = current_node;
    return newNode;
}

int total_degrees = 0;
double total_rad = 0;
int total_distance = 0;
double radius = 52.9;

Node* drive_store(int left, int right, Node* current_node) {
    Node* new_node = push(left, right, current_node);
    drive_goto(left, right);
    return new_node;
}

int newRound(double x) {
  if (x - (int) x < 0.5) return (int) x;
  else return (int) x + 1;
}

int calculateTicks(int distanceInMillimetres) {
  return newRound(distanceInMillimetres / 3.25);
}

Node* turn(int degrees, Node* current_node) {
  total_degrees += degrees;
  double radians = degrees *  PI / 180;
  total_rad += radians;
  int ticks = calculateTicks(radians * radius);
  Node* new_node = drive_store(ticks, -ticks, current_node);
  return new_node;
}

void turnWithoutStore(int degrees) {
    double radians = degrees *  PI / 180;
    int ticks = calculateTicks(radians * radius);
    drive_goto(ticks, -ticks);
}

Node* forwards(int distance, Node* current_node) {
    Node* new_node = drive_store(calculateTicks(distance), calculateTicks(distance), current_node);
    total_distance += distance;
    return new_node;
}

int irLeft, irRight;
int DO = 22, CLK = 23, DI = 24, CS = 25;
double Kp = 1;

double calculatePout(int left, int right) {
  return Kp * (right - left);
}

void writeNode(FILE* fp, Node* current_node) {
    // write left and right values of last drive_goto call with 3 digits
    fwrite("%3d%3d\n", current_node->left, current_node->right, fp); 
}

int main() {
  // drive_speed(MAX_SPEED, MAX_SPEED);
  int distance, irLeft, irRight;

  low(26);  
  low(27);

  Node* current_node = malloc(sizeof(Node));
  current_node->left = 0;
  current_node->right = 0;
  current_node->next = NULL;
  
  while(true) {
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

    double Pout = calculatePout(irLeft, irRight);

    current_node = turn(Pout, current_node);
    
    current_node = forwards(FWD, current_node);
    
    distance = ping_cm(8);
    if (distance <= 10) {
        printf("Angle travelled is %d. Total displacement is %d.\n", total_degrees, newRound(total_distance / total_rad));
        high(26);
        pause(100);
        low(26);
        break;
    } 
  }
    turnWithoutStore(179);
  
    while (current_node != NULL) {
        if (!(current_node->right == 0 && current_node->left == 0)) {
            drive_goto(current_node->right, current_node->left);
        }
        current_node = current_node->next;
    }
}
