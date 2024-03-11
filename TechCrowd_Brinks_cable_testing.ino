#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const int numConnections = 9;
const int connections[numConnections][2] = {{2, 22}, {3, 24}, {4, 26}, {5, 28}, {6, 30}, {7, 32}, {8, 34}, {9, 36}, {10, 38}};
const int numConnectionsRJ = 4;
const int rjConnections[numConnectionsRJ][2] = {{3, 40}, {4, 42}, {5, 44}, {6, 46}};
const int greenLED = 48;
const int redLED = 50;
const int buzzerPin = 13;
const int pushButtonPin1 = 52;
const int pushButtonPin2 = 53;
const int GND1 = A0; // Analog pin A0 used as GND
const int GND2 = A1; // Analog pin A1 used as GND
const int GND3 = A2; // Analog pin A2 used as GND
const int OLED_RESET = -1;

bool testWires1 = false;
bool testWires2 = false;

Adafruit_SSD1306 display(OLED_RESET);

struct Error {
  int senderPin;
  int receiverPin;
};

Error errors[10];
int errorCount = 0;
int currentError = 0;

// Function prototypes
void displayResults(const char* testType, int errorCount);
void displayInitialMessage();

void setup() {
  Serial.begin(9600);
  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(pushButtonPin1, INPUT_PULLUP);
  pinMode(pushButtonPin2, INPUT_PULLUP);
  pinMode(GND1, OUTPUT);
  digitalWrite(GND1, LOW);

  pinMode(GND2, OUTPUT);
  digitalWrite(GND2, LOW);

  pinMode(GND3, OUTPUT);
  digitalWrite(GND3, LOW);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for (;;);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  for (int i = 0; i < numConnections; i++) {
    int senderPin = connections[i][0];
    int receiverPin = connections[i][1];

    pinMode(senderPin, OUTPUT);
    digitalWrite(senderPin, LOW);

    pinMode(receiverPin, INPUT);
    digitalWrite(receiverPin, LOW);
  }

  for (int i = 0; i < numConnectionsRJ; i++) {
    int senderPin = rjConnections[i][0];
    int receiverPin = rjConnections[i][1];

    pinMode(senderPin, OUTPUT);
    digitalWrite(senderPin, LOW);

    pinMode(receiverPin, INPUT);
    digitalWrite(receiverPin, LOW);
  }

  displayTechCrowdAnimation(); // Display the animation
}

void loop() {
  if (digitalRead(pushButtonPin1) == LOW) {
    testWires1 = true;
    delay(100);
  }

  if (digitalRead(pushButtonPin2) == LOW) {
    testWires2 = true;
    delay(100);
  }

  if (testWires1) {
    testRStoRS();
    testWires1 = false;
  }

  if (testWires2) {
    testRStoRJ();
    testWires2 = false;
  }

  delay(1000);
}

void testRStoRS() {
  errorCount = 0;
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Testing RStoRS wires...");
  display.display();
  
  for (int i = 0; i < numConnections; i++) {
    int senderPin = connections[i][0];
    int receiverPin = connections[i][1];

    digitalWrite(senderPin, HIGH);
    delay(50);
    int receiverState = digitalRead(receiverPin);

    if (receiverState == HIGH) {
      Serial.print("Pin ");
      Serial.print(senderPin);
      Serial.print(" connected to Pin ");
      Serial.println(receiverPin);
    } else {
      errors[errorCount].senderPin = senderPin;
      errors[errorCount].receiverPin = receiverPin;
      errorCount++;
    }

    digitalWrite(senderPin, LOW);
    delay(50);
  }

  if (errorCount == 0) {
    digitalWrite(greenLED, HIGH);
    digitalWrite(redLED, LOW);
    tone(buzzerPin, 1000, 200);
  } else {
    digitalWrite(greenLED, LOW);
    digitalWrite(redLED, HIGH);
    tone(buzzerPin, 1000, 1000);
  }

  displayResults("RStoRS", errorCount);
  delay(2000);

  digitalWrite(greenLED, LOW);
  digitalWrite(redLED, LOW);
  noTone(buzzerPin);
  displayInitialMessage();
}

void testRStoRJ() {
  errorCount = 0;
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Testing RStoRJ wires...");
  display.display();
  
  for (int i = 0; i < numConnectionsRJ; i++) {
    int senderPin = rjConnections[i][0];
    int receiverPin = rjConnections[i][1];

    digitalWrite(senderPin, HIGH);
    delay(50);
    int receiverState = digitalRead(receiverPin);

    if (receiverState == HIGH) {
      Serial.print("Pin ");
      Serial.print(senderPin);
      Serial.print(" connected to Pin ");
      Serial.println(receiverPin);
    } else {
      errors[errorCount].senderPin = senderPin;
      errors[errorCount].receiverPin = receiverPin;
      errorCount++;
    }

    digitalWrite(senderPin, LOW);
    delay(50);
  }

  if (errorCount == 0) {
    digitalWrite(greenLED, HIGH);
    digitalWrite(redLED, LOW);
    tone(buzzerPin, 1000, 200);
  } else {
    digitalWrite(greenLED, LOW);
    digitalWrite(redLED, HIGH);
    tone(buzzerPin, 1000, 1000);
  }

  displayResults("RStoRJ", errorCount);
  delay(2000);

  digitalWrite(greenLED, LOW);
  digitalWrite(redLED, LOW);
  noTone(buzzerPin);
  display.clearDisplay();
  displayInitialMessage();
  display.display();
}

void displayResults(const char* testType, int errorCount) {
  display.clearDisplay();
  int textX = 2; // Adjust the x-coordinate to move the text away from the left side
  int textY = 2; // Adjust the y-coordinate to move the text away from the top
  
  display.drawRect(0, 0, display.width(), display.height(), SSD1306_WHITE);
  display.setCursor(textX, textY);
  display.println("Test Type: ");
  display.setCursor(textX + 70, textY); // Move the second column to the right
  display.println(testType);
  display.setCursor(textX, textY + 10); // Move to the next row
  display.println("Error Count: ");
  display.setCursor(textX + 70, textY + 10); // Move the second column to the right
  display.println(errorCount);
  
  if (errorCount > 0) {
    display.print("Error Details:");
    display.display();
    delay(2000);

    for (int i = 0; i < errorCount; i++) {
      display.clearDisplay();

      
      display.setCursor(5, 8);
      display.print("Pin ");
      display.print(errors[i].senderPin);
      display.print(" NOT connected  to Pin ");
      display.println(errors[i].receiverPin);
      display.drawRect(0, 0, display.width(), display.height(), SSD1306_WHITE);
      display.display();
      delay(2000);
    }
  }

  display.display();
}

void displayInitialMessage() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(5, 5);
  display.println("Zvolte test stiskem");
  display.setCursor(5, 15);
  display.println("tlacitka");
  display.drawRect(0, 0, display.width(), display.height(), SSD1306_WHITE);
  
  int textX = display.width() - 55; // Adjust the x-coordinate to move the text from the right
  int textY = display.height() - 8; // Adjust the y-coordinate to move the text from the bottom

  display.setTextSize(1);
  display.setCursor(textX, textY);
  display.println("TechCrowd");

  display.display();
}
void displayTechCrowdAnimation() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);

  int textWidth = 6 * 16; 
  int textHeight = 16; 
  int textX = (display.width() - textWidth) / 2; 
  int textMaxY = display.height() - textHeight; 
  int textY = textMaxY; 

  display.drawRect(0, 0, display.width(), display.height(), SSD1306_WHITE);

  for (int i = 0; i < 2; i++) { 
    for (int y = textMaxY; y > textMaxY - 20; y--) { // Move the text up
      display.clearDisplay();
      display.setCursor(textX, y);
      display.println("TechCrowd");
      display.display();
      delay(3); // Adjust the delay to control the speed of the animation
    }

    for (int y = textMaxY - 20; y <= textMaxY; y++) { // Move the text down
      display.clearDisplay();
      display.setCursor(textX, y);
      display.println("TechCrowd");
      display.display();
      delay(3); // Adjust the delay to control the speed of the animation
    }
  }

  delay(1000); // Pause for 1 second before showing the initial message
  displayInitialMessage();
}
