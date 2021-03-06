
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

double newAbs(double x) {
    return (x >= 0) ? x : -x;
}

int calculateTicks(int distanceInMillimetres) {
  return newRound(distanceInMillimetres / 3.25);
}

Node* turn(int degrees, Node* current_node) {
  double radians = degrees *  PI / 180.0;
  int ticks = calculateTicks(radians * radius);
  Node* new_node = drive_store(ticks, -ticks, current_node);
  return new_node;
}

int getTurnAmount(int degrees) {
    double radians = degrees *  PI / 180;
    return newAbs(calculateTicks(radians * radius));
}

void turnWithoutStore(int degrees) {
    double radians = degrees *  PI / 180;
    int ticks = calculateTicks(radians * radius);
    drive_goto(ticks, -ticks);
}

Node* forwards(int distance, Node* current_node) {
    Node* new_node = drive_store(calculateTicks(distance), calculateTicks(distance), current_node);
    return new_node;
}

void printStack(Node* top_node) {
    int number = 1;
    while (top_node != NULL) {
        printf("Node #%d: Left %d, Right: %d.\n", number++,
        top_node->left, top_node->right);
        top_node = top_node->next;
    }
}

void printNode(Node* node) {
    printf("Storing... Left: %d, Right: %d.\n", node->left, node->right);
}

void freeStack(Node* top_node) {
    Node* temp;
    while (top_node != NULL) {
        temp = top_node->next;
        free(top_node);
        top_node = temp;
    }
}

int irLeft, irRight;
double epsilon = 0.001f;
double Kp = 0.9;

double calculatePout(int left, int right) {
  return Kp * (right - left);
}

int main() {
  int distance, irLeft, irRight;

  low(26);
  low(27);

  Node* top_node = malloc(sizeof(Node));
  top_node->left = 0;
  top_node->right = 0;
  top_node->next = NULL;

  Node* current_node = top_node;
  int temp_sum = 0;
  int turned; // undercover boolean
  int initialSpeed = 100;

  while(true) {
    turned = false;
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
    printf("%d\n", (int) 128 + (Pout));
    int leftspeed = initialSpeed, rightspeed = initialSpeed;
    if (Pout < 0) {
        leftspeed = initialSpeed + Pout;
        rightspeed = initialSpeed - Pout;
    } else {
        leftspeed = initialSpeed + Pout;
        rightspeed = initialSpeed - Pout;
    }
    drive_speed(leftspeed , rightspeed);
    pause(500);
    current_node = push(leftspeed, rightspeed, current_node);
    distance = ping_cm(8);
    if (distance <= 10) {
        high(26);
        pause(100);
        low(26);
        break;
    }
    continue;
    if (getTurnAmount(Pout) > 0) {
        current_node = turn(Pout, current_node);
        printNode(current_node);
        turned = true;
    }

    temp_sum += calculateTicks(FWD);
    printf("Incrementing temp_sum... %d\n", temp_sum);
    drive_goto(calculateTicks(FWD), calculateTicks(FWD));

    if (turned) {
        turned = false;
        printf("Storing temp_sum %d\n", temp_sum);
        current_node = push(temp_sum, temp_sum, current_node);
        temp_sum = 0;
        printNode(current_node);
    }

    distance = ping_cm(8);
    if (distance <= 10) {
        if (temp_sum > 0) {
            current_node = push(calculateTicks(temp_sum), calculateTicks(temp_sum), current_node);
        }
        high(26);
        pause(100);
        low(26);
        break;
    }
  }
    top_node = current_node;

    turnWithoutStore(179);

    while (current_node->next != NULL) {
        drive_goto(current_node->right, current_node->left);
        current_node = current_node->next;
        printStack(current_node);
    }
    freeStack(top_node);
}
