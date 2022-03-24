const int PROGRAM_PIN = A4;
const int DATA_PINS[] = {9, 8, 7, 6, 5, 4, 3, 2}; // LSB first
const int ADDR_PINS[] = {A3, A2, A1, A0, 13, 12, 11, 10}; // LSB first

void preset() {
  for (int i = 0; i < 8; i++) {
    pinMode(DATA_PINS[i], OUTPUT);
    pinMode(ADDR_PINS[i], OUTPUT);
  }
  pinMode(PROGRAM_PIN, OUTPUT);
  digitalWrite(PROGRAM_PIN, HIGH);
  Serial.begin(115200);
}

void writeToPins(const int pins[], uint8_t data) {
  for (int i = 0; i < 8; i++) {
    digitalWrite(pins[i], data & 1);
    data >>= 1;
  }
}

const uint8_t DATA[] = {
  #include "to_write.h"
  // This is from another file.
};
const int NUM_ENTRIES = sizeof(DATA) / sizeof(uint8_t);


void program() {
  for (int i = 0; i < NUM_ENTRIES; i++) {
    int data = DATA[i];
    writeToPins(ADDR_PINS, i);
    writeToPins(DATA_PINS, data);
    delay(5);
    digitalWrite(PROGRAM_PIN, HIGH);
    delay(5);
    digitalWrite(PROGRAM_PIN, LOW);
    delay(5);
    digitalWrite(PROGRAM_PIN, HIGH);
  }
}

void setup() {
  preset();
  program();
}


void loop() {
  
}
