#include <Arduino.h>
#include <Adafruit_CAP1188.h>
Adafruit_CAP1188 cap = Adafruit_CAP1188();

#define RED_PIN 32
#define YELLOW_PIN 33
#define GREEN_PIN 25
#define BUZZER_PIN 26

// Hard coded enumerator
#define RED_STATE 0
#define RED_YELLOW_STATE 1
#define YELLOW_STATE 2
#define GREEN_STATE 3

#define RED_MILLIS 10000
#define YELLOW_MILLIS 2000
#define GREEN_MILLIS 5000

#define BUZZER_GREEN_ON 500
#define BUZZER_GREEN_OFF 1500
#define BUZZER_RED 250

int tl_state;           // Traffic light state.
unsigned long tl_timer; // Traffic light timer.
unsigned long buzzer_timer;
bool buzzer_state;
bool buttonPressed = false;

void setup()
{
  Serial.begin(9600);
  // Configure LED pins as outputs. 
  pinMode(RED_PIN, OUTPUT); 
  pinMode(YELLOW_PIN, OUTPUT); 
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  if (!cap.begin()) {
    Serial.println("CAP1188 not found");
    while (1);
  }
  Serial.println("CAP1188 found!");

  // Initial state for states and timers.. 
  tl_state = RED_STATE;
  tl_timer = millis() + RED_MILLIS;

  buttonPressed = false;

  digitalWrite(BUZZER_PIN, LOW);
  buzzer_timer = millis() + BUZZER_RED;
  buzzer_state = 1;
}

void loop()
{
  // TL state machine
  switch (tl_state) {
    
    case RED_STATE:
        // Turn red light on.
        digitalWrite(RED_PIN, HIGH);
        // buzzer_timer = millis() + BUZZER_RED;
        // digitalWrite(BUZZER_PIN, HIGH);
        if (millis() >= tl_timer/*Timer expired*/)
        { 
            // Set timer for red-yellow state.
            tl_state = RED_YELLOW_STATE;
            tl_timer += YELLOW_MILLIS;
        }
        if(millis() >= buzzer_timer){
          buzzer_state = 1;
          buzzer_timer = BUZZER_RED + millis();
        }
        while(millis() < buzzer_timer && buzzer_state){
          digitalWrite(BUZZER_PIN, HIGH);
          delay(1);
          digitalWrite(BUZZER_PIN, LOW);
          delay(1);
        }
        if(buzzer_state){
          buzzer_timer = millis() + BUZZER_RED;
          buzzer_state = 0;
        }
        break;
    
    case RED_YELLOW_STATE:
        // Code for red-yellow state.
        digitalWrite(YELLOW_PIN, HIGH);
        if (millis() >= tl_timer/*Timer expired*/)
        { 
            // Turn red light off.
            digitalWrite(RED_PIN, LOW);
            // Set timer for red-yellow state.
            tl_state = YELLOW_STATE;
            tl_timer += YELLOW_MILLIS;
            buzzer_timer = millis() + BUZZER_RED;
        }
        break;

    case YELLOW_STATE:
        // Code for yellow state.
        if(buttonPressed){
          buttonPressed = false;
          delay(YELLOW_MILLIS);
          digitalWrite(YELLOW_PIN, LOW);
          tl_timer = millis() + RED_MILLIS;
          tl_state = RED_STATE;
        }
        if (millis() >= tl_timer/*Timer expired*/)
        { 
            // Turn red light off.
            digitalWrite(YELLOW_PIN, LOW);
            // Set timer for red-yellow state.
            tl_state = GREEN_STATE;
        }
    break;
    
    case GREEN_STATE:
        // Turn green light on.
        digitalWrite(GREEN_PIN, HIGH);
        if (buttonPressed && millis() >= tl_timer)
        {
            // Turn green light off.
            digitalWrite(GREEN_PIN, LOW);
            // Set timer for yellow 
            tl_state = YELLOW_STATE;
            digitalWrite(YELLOW_PIN, HIGH);
            tl_timer += YELLOW_MILLIS;
        }
        if(buttonPressed){
          if(millis() >= buzzer_timer){
            buzzer_state = 1;
            buzzer_timer = BUZZER_GREEN_ON + millis();
          }
          while(millis() < buzzer_timer && buzzer_state){
            digitalWrite(BUZZER_PIN, HIGH);
            delay(1);
            digitalWrite(BUZZER_PIN, LOW);
            delay(1);
          }
          if(buzzer_state){
            buzzer_timer = millis() + BUZZER_GREEN_OFF;
            buzzer_state = 0;
          }
        }
        break;
    }
    
    // Detect touch - button pressed.
    if(cap.touched() && !buttonPressed && tl_state == GREEN_STATE){
        buzzer_state = 1;
        buttonPressed = true;
        tl_timer = millis() + GREEN_MILLIS;
        buzzer_timer = BUZZER_GREEN_ON + millis();
    }
}