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
const int total_seconds = 30;
int seconds; 
int seconds_over = 0;
bool halfway;
bool finished;

// Define the array that decides who's turn it is to go (order generated randomly beforehand).
String bin_takers[10] = {"James W", "Immy", "Mya", "James H", "Louis", "Liv", "Carl", "Lucy", "Sam", "Anna"};
int index;

LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

void setup() {
  // DEBUGGING REMOVE LATER
  Serial.begin(115200);
  Serial.print("HELLOOOO");
  
  // Set number of rows and columns in LCD.
  lcd.begin(16, 2);
  delay(1000);

  // Set the inital value of countdown.
  seconds = total_seconds;
  halfway = false;
  finished = false;

  // Set up the LEDS, buzzer and button.
  digitalWrite(RED_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(GREEN_LED, HIGH);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Get the index from EEPROM if its there, otherwise make it again.
  // REMEMBER TO ADD THAT BTW I KNOW YOUR BEING LAZY RN.
  index = 0;
  
}

void loop() {
  if (!finished) {
    // Decrement the timer.
    seconds -= 1;
    Serial.println(seconds);

    if (seconds <= 0) {
      seconds = 0;
      finished = true;
      Serial.println("RED");
      digitalWrite(RED_LED, HIGH);
      digitalWrite(YELLOW_LED, LOW);
      digitalWrite(GREEN_LED, LOW);

      lcd.setCursor(0, 1);
      lcd.print("It is " + bin_takers[index] + "'s");
      lcd.setCursor(0, 2);
      lcd.print("turn to take bin");
       
    } else if (!halfway && seconds <= total_seconds / 2) {
      halfway = true;
      digitalWrite(RED_LED, LOW);
      digitalWrite(YELLOW_LED, HIGH);
      digitalWrite(GREEN_LED, LOW);
      Serial.println("YELLOW");
    }
    
  } else {
    // Keep track of seconds gone over, to prevent the alarm trigger shifting into unreasonable hours of the night.
    seconds_over ++;
    // Play the note C5 through pin D8 for 200ms.
    tone(BUZZER_PIN, NOTE_C5, 200);
    
    // If the timer has finished, check for button press to reset.
    if (digitalRead(BUTTON_PIN) == LOW) {
      seconds = total_seconds - seconds_over;
      seconds_over = 0;
      finished = false;
      halfway = false;
      digitalWrite(RED_LED, LOW);
      digitalWrite(YELLOW_LED, LOW);
      digitalWrite(GREEN_LED, HIGH);
      Serial.println("GREEN");
    }
  }
  
  delay(1000);
  
}