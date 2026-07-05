// Include libraries
#include <Wire.h>                            // Include library for I2C communication (RTC and LCD)
#include <RTClib.h>                           // Include library for RTC
#include <LiquidCrystal_I2C.h>                // Include library for LCD

// Define Objects
RTC_DS3231 rtc;                               // Define RealTimeClock (RTC)
LiquidCrystal_I2C lcd(0x27, 16, 2);           // Define Liquid Crystal Display (LCD)

// Define Pins
const int relayPin = 2;                       // Pin for lock Relay

// Standard time settings (Changeable via App)
int feedingHour = 5;                          // Standard feeding time: 5 AM
int feedingDuration = 2000;                   // Voltage high standard 2000ms = 2 seconds

// Standard system settings 
bool systemOn = true;                         // Main switch on/off
bool feedingAccomplished = false;             // True if the feeding action has been completed for today

// Define RTC time as boolean
bool isItFeedingTime() {
  DateTime now = rtc.now();
  return (now.hour() == feedingHour && now.minute() == 0 && now.second() == 0);
} 

// Checks if it is midnight for the status reset
bool isItMidnight() {
  DateTime now = rtc.now();
  return (now.hour() == 0 && now.minute() == 0 && now.second() == 0);
}


void setup() {
  // Start up the system
  lcd.init();                                 // Start LCD-screen
  lcd.backlight();
  lcd.print("Hello!");

  rtc.begin();                                // Start Real Time clock

  pinMode(relayPin, OUTPUT);                  // Set relay-pin up as output
  digitalWrite(relayPin, HIGH);               // Set relay standard to HIGH (de-energized for active-low relay)

  delay(5000);                                // Wait for 5 seconds 
  lcd.clear();                                // Clear LCD screen
}


void loop() {
  // Real Time Check
  DateTime now = rtc.now();                   // Check RTC time

  // Run our 3 hardware folders (logic blocks)
  runRelayLogic();                            
  checkMidnightReset();                       
  updateLcdDisplay(now);                      

  delay(200);                                 // Delay to prevent LCD from blinking
}

void runRelayLogic() {                        // Run relay folder
  if (!feedingAccomplished && systemOn && isItFeedingTime()) {
    digitalWrite(relayPin, LOW);              // Turn relay ON (voltage LOW for active-low relay)
    delay(feedingDuration);                   // Wait for standard duration (2 seconds)
    digitalWrite(relayPin, HIGH);             // Turn relay OFF (voltage HIGH for active-low relay)

    feedingAccomplished = true;               // Feeding completed for today
    lcd.clear();                              // Clear LCD screen
  }
}

void checkMidnightReset() {                   // Run reset LCD on midnight folder
  if (isItMidnight()) {
    feedingAccomplished = false;              // Reset the feeding completed status
  }
}

void updateLcdDisplay(DateTime timeNow) {     // Run LCD print folder
  // LCD screen first row print
  lcd.setCursor(0, 0);                        // Set cursor top left
    
  if (!feedingAccomplished) {                 // Check if feeding is completed
    lcd.print(feedingHour);                   
  } else {                                    // Print on LCD screen
    lcd.print("Feeding Completed ");   
  }

  // LCD Print second row
  lcd.setCursor(0, 1);                        // Set cursor bottom left

  char timeBuffer[17];                        // Char array / buffer to print out time readable 
  snprintf(timeBuffer, sizeof(timeBuffer), "Time:    %02d:%02d:%02d", timeNow.hour(), timeNow.minute(), timeNow.second());

  lcd.print(timeBuffer);                      // Print real time on LCD screen
}
