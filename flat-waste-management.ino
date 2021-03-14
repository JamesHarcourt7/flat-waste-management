#include <EEPROM.h>
#include <LiquidCrystal.h>
#include <pitches.h>

// Define constants for all the pins.
const int BUTTON_PIN = 2;
const int BUZZER_PIN = 3;
const int RED_LED = 4;
const int YELLOW_LED = 5;
const int GREEN_LED = 6;

// Timer variables. There are 172800 seconds in 48 hours.
const long total_seconds = 30;
int seconds; 
int seconds_over = 0;
bool halfway;
bool finished;

// Define the array that decides who's turn it is to go (order generated randomly beforehand).
String bin_takers[10] = {"James W", "Immy", "Mya", "James H", "Louis", "Liv", "Carl", "Lucy", "Sam", "Anna"};
// Save current index in EEPROM, to avoid someone resetting it by mistake (or on purpose).
int check_val = 69;
int index;
byte quarters;
int check_addr = 0;
int index_addr = 1;
int time_addr = 2;

LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

void setup() {
  // Set number of rows and columns in LCD.
  lcd.begin(16, 2);
  delay(1000);

  // Set up the LEDS, buzzer and button.
  digitalWrite(RED_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(GREEN_LED, HIGH);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Get the index from EEPROM if its there, otherwise make it again.
  if (EEPROM.read(check_addr) == check_val) {
    index = EEPROM.read(index_addr);
    // Multiply time value by 900 as it is stored as 15 minute intervals (to keep it under a byte and reduce amount of writing).
    seconds = EEPROM.read(time_addr) * 900;
    
    if (seconds >= total_seconds / 2) {
      halfway = false;
      finished = false;
    } else if (seconds == 0) {
      halfway = true;
      finished = true;
    } else {
      halfway = true;
      finished = false;
    }
    
  } else {
    // Initially set index as 0 and seconds as total, and write these to EEPROM. Set check value as well.
    index = 0;
    // Set the inital value of countdown.
    seconds = total_seconds;
    halfway = false;
    finished = false;

    // To keep the value under a byte (I can't be bothered to convert long to 3 bytes), we will store time as the number of 15 minute intervals remaining from the total.
    // 172800 / 900 = 192
    quarters = 192;
    
    EEPROM.write(check_addr, check_val);
    EEPROM.write(index_addr, 0);
    EEPROM.write(time_addr, quarters);
    
    
  }
  delay(1000);
}

void loop() {
  if (!finished) {
    // Decrement the timer.
    seconds -= 1;

    // Check if seconds divisible by 900 (is a multiple of 15 in minutes).
    if (seconds % 900 == 0) {
      // Write new value to EEPROM.
      quarters = seconds / 900;
      EEPROM.write(time_addr, quarters);
    }

    // Display current time and previous person displayed when button is pressed.
    if (digitalRead(BUTTON_PIN) == LOW) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Countdown: " + seconds);
    }

    if (seconds <= 0) {
      seconds = 0;
      finished = true;
      digitalWrite(RED_LED, HIGH);
      digitalWrite(YELLOW_LED, LOW);
      digitalWrite(GREEN_LED, LOW);

      lcd.setCursor(0, 0);
      lcd.clear();
      lcd.print("It is " + bin_takers[index] + "'s");
      lcd.setCursor(0, 1);
      lcd.print("turn to take bin");
       
    } else if (!halfway && seconds <= total_seconds / 2) {
      halfway = true;
      digitalWrite(RED_LED, LOW);
      digitalWrite(YELLOW_LED, HIGH);
      digitalWrite(GREEN_LED, LOW);
    }
    
  } else {
    // Keep track of seconds gone over, to prevent the alarm trigger shifting into unreasonable hours of the night.
    seconds_over ++;
    // Play the note C5 through pin D8 for 200ms.
    tone(BUZZER_PIN, NOTE_C5, 200);
    
    // If the timer has finished, check for button press to reset.
    if (digitalRead(BUTTON_PIN) == LOW) {
      index ++;
      if (index == 10) index = 0;
      // Update the value of index in EEPROM, in case of power down.
      EEPROM.write(index_addr, index);
      seconds = total_seconds - seconds_over;
      seconds_over = 0;
      finished = false;
      halfway = false;
      digitalWrite(RED_LED, LOW);
      digitalWrite(YELLOW_LED, LOW);
      digitalWrite(GREEN_LED, HIGH);
    }
  }
  
  delay(1000);
  
}
