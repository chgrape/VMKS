#include <SoftwareSerial.h>

SoftwareSerial BTM(10,11);

const char CLEAR = 0;
const char DOT = 1;
const char DASH = 2;
const char alphabet[26][6] {
  { 'A', DOT, DASH, CLEAR, CLEAR, CLEAR},
  { 'B', DASH, DOT, DOT, DOT, CLEAR},
  { 'C', DASH, DOT, DASH, DOT, CLEAR},
  { 'D', DASH, DOT, DOT, CLEAR, CLEAR},
  { 'E', DOT, CLEAR, CLEAR, CLEAR, CLEAR},
  { 'F', DOT, DOT, DASH, DOT, CLEAR},
  { 'G', DASH, DASH, DOT, CLEAR, CLEAR},
  { 'H', DOT, DOT, DOT, DOT, CLEAR},
  { 'I', DOT, DOT, CLEAR, CLEAR, CLEAR},
  { 'J', DOT, DASH, DASH, DASH, CLEAR},
  { 'K', DASH, DOT, DASH, CLEAR, CLEAR},
  { 'L', DOT, DASH, DOT, DOT, CLEAR},
  { 'M', DASH, DASH, CLEAR, CLEAR, CLEAR},
  { 'N', DASH, DOT, CLEAR, CLEAR, CLEAR},
  { 'O', DASH, DASH, DASH, CLEAR, CLEAR},
  { 'P', DOT, DASH, DASH, DOT, CLEAR},
  { 'Q', DASH, DASH, DOT, DASH, CLEAR},
  { 'R', DOT, DASH, DOT, CLEAR, CLEAR},
  { 'S', DOT, DOT, DOT, CLEAR, CLEAR},
  { 'T', DASH, CLEAR, CLEAR, CLEAR, CLEAR},
  { 'U', DOT, DOT, DASH, CLEAR, CLEAR},
  { 'V', DOT, DOT, DOT, DASH, CLEAR},
  { 'W', DOT, DASH, DASH, CLEAR, CLEAR},
  { 'X', DASH, DOT, DOT, DASH, CLEAR},
  { 'Y', DASH, DOT, DASH, DASH, CLEAR},
  { 'Z', DASH, DASH, DOT, DOT, CLEAR}
};

const unsigned long dotDuration = 1000;
const unsigned long tolerance = 1000;
const unsigned long dashDuration = 2 * dotDuration;
const unsigned long shortGap = dotDuration;
const unsigned long mediumGap = 3 * dotDuration;
const unsigned long longGap = 7 * dotDuration;

enum State { 
  UP = 1, 
  DOWN = 0 } state;

unsigned long lastChange;

unsigned long downDuration;

char character[5];

int characterIndex;

enum Action {
  START = 0,
  READ_DASHDOT = 1,
  READ_CHARACTER = 2,
  READ_WORD = 3
} action;

int buttonPin = 6;
int togglePin = 7;
int servoPin = 9;

int toggle = 0;

Servo s;

void setup() {
  Serial.begin(9600);
  BTM.begin(9600);

  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(togglePin, INPUT_PULLUP);

  s.attach(servoPin);

  state = UP;
  characterIndex = 0;
  downDuration = 0;
  lastChange = 0;
  action = START;
}

void clearCharacter() {
  characterIndex = 0;
  for (int i = 0; i < 5; ++i) {
    character[i] = CLEAR;
  }
}

void readDashDot() {
  
  if(downDuration < 10){
    return;
  }

  if (downDuration >= dashDuration - tolerance && downDuration < dashDuration + tolerance) {
    character[characterIndex] = DASH;
    characterIndex++;
  } else if (downDuration >= dotDuration - tolerance && downDuration < dotDuration + tolerance) {
    character[characterIndex] = DOT;
    characterIndex++;
  } else {
    BTM.println("Down duration unrecognized");
  }
}

char readCharacter() {
  bool found;
  for (int i = 0; i < 26; ++i) {
    found = true;
    for (int j = 0; found && j < 5; ++j) {
      if (character[j] != alphabet[i][j + 1]) {
        found = false;
      }
    }
    if (found) {
      return alphabet[i][0];
    }
  }
  return 0;
}

void loop() {
  int toggleState = digitalRead(togglePin);
  
  if(toggleState == LOW && toggle == 0){
    toggle = 1;
    delay(500);
  }else if(toggleState == LOW && toggle == 1){
    toggle = 0;
    delay(500);
  }    
  
  if(toggle == 0){
    int buttonState = digitalRead(buttonPin);
    if(buttonState == LOW){
      myservo.writeMicroseconds(950);
      delay(200);
      myservo.writeMicroseconds(1000);
    }
  }else{
    State newState = digitalRead(buttonPin) ? UP : DOWN;

    if (newState == state) {
      if (newState == UP) {
        unsigned long upDuration = (millis() - lastChange);
        if (action == READ_DASHDOT && upDuration >= shortGap - tolerance && upDuration < shortGap + tolerance) {
          readDashDot();
          action = READ_CHARACTER;
        } else if (action == READ_CHARACTER && upDuration >= mediumGap - tolerance && upDuration < mediumGap + tolerance) {
          char c = readCharacter();
          if (c != 0) {
            BTM.println(c);
          } else {
            BTM.println("Unrecognized character");
          }
          clearCharacter();
          action = READ_WORD;
        } else if (action == READ_WORD && upDuration >= longGap - tolerance && upDuration < longGap + tolerance) {
          action = READ_DASHDOT;
        }
      } else {
        downDuration = (millis() - lastChange);
      }
    } else {
      if (state == UP && newState == DOWN) {
        downDuration = 0;
      }
      lastChange = millis();
      state = newState;
      action = READ_DASHDOT;
    }
  }
}
