#include <Adafruit_NeoPixel.h>
#include <NewPing.h>

// Define the pins for the NeoPixel strips and the button
const int stripPin1 = 9; // Pin for the first NeoPixel strip
const int stripPin2 = 10; // Pin for the second NeoPixel strip
const int buttonPin = 2;  // Pin for the button
#define TRIG_PIN 5
#define ECHO_PIN 4
#define MAX_DISTANCE 100 // Maximum distance in centimeters (adjust as needed)
#define NUM_LEDS 8       // Number of LEDs in your strip (adjust as needed)

#define NUM_LEDS 8 // Number of LEDs in each strip

Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(NUM_LEDS, stripPin1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(NUM_LEDS, stripPin2, NEO_GRB + NEO_KHZ800);

// Define the durations in milliseconds
const unsigned long studyDuration = 35 * 60 * 1000UL; // 35 minutes in milliseconds
const unsigned long breakDuration = 10 * 60 * 1000UL;    // 10 minutes in milliseconds

unsigned long previousMillis = 0;
bool studyTime = true;
bool flashing = false;
unsigned long flashInterval = 100; // Flash interval in milliseconds
unsigned long flashMillis = 0;
int numLedsOn = NUM_LEDS;
unsigned long ledTurnOffInterval = studyDuration / NUM_LEDS;
unsigned long lastLedTurnOffMillis = 0;
bool tooFar = false;

NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE);

void setup() {
  pinMode(stripPin1, OUTPUT);
  pinMode(stripPin2, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP); // Use internal pull-up resistor for the button
  strip1.begin();
  strip2.begin();
  strip1.show(); // Initialize all pixels to 'off'
  for(int i = 0; i < strip2.numPixels(); i++) {
      strip2.setPixelColor(i, strip2.Color(0, 0, 255) );
      strip2.setBrightness(20);
   }
  strip2.show();
  Serial.begin(9600);
}

void loop() {
  unsigned long currentMillis = millis();
  unsigned long remainingTime;

  // Check if the button is pressed to dismiss the flashing
  if (flashing && digitalRead(buttonPin) == LOW) {
    flashing = false;
    strip1.fill(strip1.Color(0, 0, 0)); // Turn off all LEDs in the strip
    for(int i = 0; i < strip2.numPixels(); i++) {
      strip2.setPixelColor(i, strip2.Color(0, 0, 255) );
      strip2.setBrightness(20);
    }
    strip1.show();
    strip2.show();
    while (digitalRead(buttonPin) == LOW) {
    } // Wait for the button to be released
  }

  // Handle flashing if it's enabled
  if (flashing) {
    if (currentMillis - flashMillis >= flashInterval) {
      if (currentMillis - flashMillis >= flashInterval) {
      // Toggle the LEDs every flashInterval milliseconds
      strip1.fill(strip1.Color(255, 255, 255)); // White
      strip1.show();
      strip1.fill(strip1.Color(0, 0, 255)); // Blue
      strip1.show();
      strip1.fill(strip1.Color(0, 0, 0)); // Turn off all LEDs in the strip
       
      strip1.show();
      flashMillis = currentMillis;
    }
    }
  } else {
      if(studyTime){
        delay(100); // Wait a short time to stabilize readings
        unsigned int distance = sonar.ping_cm();
        if (distance != 0) {
          Serial.print("Distance: ");
          Serial.print(distance);
          Serial.println(" cm");

          if (distance >= 80) { // User is within 1 meter
            tooFar = true;
            flashRandomColors(10, 500); // Flash LEDs with random colors and delays
          } else {
            tooFar = false;
          }
        } else {
          tooFar = false;
          //flashRandomColors(10, 500); // Flash LEDs with random colors and delays
          //Serial.println("unidentified");
        }
      }
      // Control the LEDs based on the time
      if (studyTime && (currentMillis - previousMillis < studyDuration)) {
        remainingTime = studyDuration - (currentMillis - previousMillis);
        strip1.fill(strip1.Color(255, 255, 255)); // White for the first strip
        strip1.show();
      } else if (!studyTime && (currentMillis - previousMillis < breakDuration)) {
        remainingTime = studyDuration - (currentMillis - previousMillis);
        strip1.fill(strip1.Color(0, 0, 255)); // Blue for the first strip
        strip1.show();
      }

      if(!tooFar){
        // Gradually turn off individual LEDs on the LED strip
        if (numLedsOn > 0 && (currentMillis - lastLedTurnOffMillis >= ledTurnOffInterval)) {
          //strip2.setPixelColor(numLedsOn - 1, strip2.Color(0, 0, 0)); // Turn off one LED
          numLedsOn--;
          for(int i = 0; i < NUM_LEDS - numLedsOn; i++) {
            strip2.setPixelColor(i, strip2.Color(0,0,0) );
            if(numLedsOn == 1){
            strip2.setPixelColor(i, strip2.Color(255, 0, 0) );
           }
          }

          
          strip2.show();
          lastLedTurnOffMillis = currentMillis;
        }
      }

      if(!tooFar){
        if (numLedsOn == 0) {
          // Time's up, switch to the other mode
          studyTime = !studyTime;
          previousMillis = currentMillis;
          numLedsOn = NUM_LEDS;
          if (studyTime) {
            flashing = true;
            flashMillis = currentMillis;
          }
        }
      }
  }
}

void flashRandomColors(int count, int delayMs) {
  for (int i = 0; i < count; i++) {
    uint32_t color = strip2.Color(random(256), random(256), random(256));
    for(int i = 0; i < strip2.numPixels(); i++) {
      strip2.setPixelColor(i, color);
      strip2.setBrightness(250);
    }
    strip2.show();
    delay(random(10, delayMs));
    for(int i = 0; i < strip2.numPixels(); i++) {
      strip2.setPixelColor(i, 0);
      strip2.setBrightness(20);
    }
    strip2.show();
    delay(random(10, delayMs));
  }
}
