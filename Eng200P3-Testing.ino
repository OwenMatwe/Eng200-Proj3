// include the library code:
#include <LiquidCrystal.h>
#define JOYSTICK_X_PIN A0
#define JOYSTICK_Y_PIN A1
#define JOYSTICK_BUTTON_PIN 34

// Define menu options
const String MENU_OPTIONS[] = {"Bet", "Hit", "Stay"};
const int MENU_SIZE = sizeof(MENU_OPTIONS) / sizeof(MENU_OPTIONS[0]);

// Define variables
int cardTotal = 0;

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

void setup() {
  pinMode(JOYSTICK_BUTTON_PIN, INPUT);
  digitalWrite(JOYSTICK_BUTTON_PIN, HIGH);
  lcd.begin(16, 2);
  lcd.print("Welcome to the Casino");
}

void loop() {
  lcd.setCursor(0,1);
  if (digitalRead(JOYSTICK_BUTTON_PIN) == 0) {
    // Display menu
    int selectedIndex = displayMenu();
    // Perform action based on selected menu option
    switch (selectedIndex) {
      case 0:
        lcd.clear();
        lcd.print("Bet selected");
        break;
      case 1:
        lcd.clear();
        lcd.print("Hit selected");
        // Increase card total by a random value between 1 and 10
        cardTotal += random(1, 11);
        lcd.setCursor(0, 1);
        lcd.print("Total: " + String(cardTotal));
        break;
      case 2:
        lcd.clear();
        lcd.print("Stay selected");
        break;
    }
    // Wait for button release
    while (digitalRead(JOYSTICK_BUTTON_PIN) == 0);
  }
}

int displayMenu() {
  int selectedIndex = 0;
  // Display menu options
  lcd.clear();
  lcd.print("Select an option:");
  lcd.setCursor(0, 1);
  lcd.print(MENU_OPTIONS[0]);
  for (int i = 1; i < MENU_SIZE; i++) {
    lcd.print("  ");
    lcd.print(MENU_OPTIONS[i]);
  }
  // Wait for joystick movement or button press
  while (true) {
    // Check joystick X direction
    int xVal = analogRead(JOYSTICK_X_PIN);
    if (xVal < 100) {
      selectedIndex--;
      if (selectedIndex < 0) {
        selectedIndex = MENU_SIZE - 1;
      }
      displaySelectedOption(selectedIndex);
      delay(200);
    } else if (xVal > 900) {
      selectedIndex++;
      if (selectedIndex >= MENU_SIZE) {
        selectedIndex = 0;
      }
      displaySelectedOption(selectedIndex);
      delay(200);
    }
    // Check button press
    if (digitalRead(JOYSTICK_BUTTON_PIN) == 0) {
      return selectedIndex;
    }
  }
}

void displaySelectedOption(int index) {
  lcd.setCursor(0, 1);
  for (int i = 0; i < MENU_SIZE; i++) {
    if (i == index) {
      lcd.print(">");
    } else {
      lcd.print(" ");
    }
    lcd.print(MENU_OPTIONS[i]);
    lcd.print(" ");
  }
}