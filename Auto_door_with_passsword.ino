// Import libraries
#include <Keypad.h>
#include <Servo.h> 
#include <Wire.h>
#include <LiquidCrystal_I2C.h> 

LiquidCrystal_I2C lcd(0x27, 16, 2); 

Servo myservo; 
Servo myservo2;

// Store the binary representation of the custom character.
 byte va[] = {
  B00000,
  B01100,
  B10010,
  B10100,
  B01000,
  B10101,
  B10010,
  B01101
};
const byte ROWS = 4; 
const byte COLS = 4; 
char password[5];
char On_equip[]="4242"; // Password to open door
char Off_equip[]="1212"; // Password to close door
int i = 0;
int on=0;
int servo1 = 10;
int servo2 = 11;
int RedLed = 13;
int ledPin = 12;

// Define the keys on the keypad
char MatrixKey[ROWS][COLS] = 
{   
  {'*', '0', '#', 'D'},
  {'7', '8', '9', 'C'},
  {'4', '5', '6', 'B'},
  {'1', '2', '3', 'A'}
};

byte rowPins[ROWS] = {6, 7, 8, 9}; // R1, R2, R3, R4
byte colPins[COLS] = {5, 4, 3, 2}; // C1, C2, C3, C4

Keypad Mykeys = Keypad(makeKeymap(MatrixKey), rowPins, colPins, ROWS, COLS);


void setup()
{
  Serial.begin(9600);
  pinMode(RedLed,OUTPUT);
  
  // Initialize the LCD screen and set up custom characters
  lcd.init(); 
  lcd.backlight(); 
  lcd.createChar(0, va);

  // Attach Servo objects to respective pins and initialize them
  myservo.attach(servo1); 
  myservo2.attach(servo2); 
  myservo.write (0); 
  myservo2.write (85);

  // Set the LED pin as an output
  pinMode(ledPin, OUTPUT); 
  
// Sending text toLCD display
  lcd.setCursor(1, 0); // Move the cursor to column 1 row 0
  lcd.print("Team Everest");
  lcd.setCursor(0, 1);
  lcd.print(" Automatic Door");
  delay(8000);
  lcd.clear();
  lcd.print(" Enter Password");
  
}
//=================================================================================================================

void loop()
{
  // Read keypad input
  char EnterKey = Mykeys.getKey();

 if (EnterKey)
 {
    password[i]=EnterKey; 
    i++;
    on++;
    Serial.println(password);
    // Display the entered password on LCD with masking
    // '*' is displayed for each entered character
    if (i == 0) {
      password[0] = EnterKey;
      lcd.setCursor(6, 1);
      lcd.print(password[0]);
      delay(500); // Character show on LCD screen after 0.5s
      lcd.setCursor(6, 1);
      lcd.print("*"); // Character is replaced by *
    }
    if (i == 1) {
      password[1] = EnterKey;
      lcd.setCursor(7, 1);
      lcd.print(password[1]);
      delay(500);
      lcd.setCursor(7, 1);
      lcd.print("*");
    }
    if (i == 2) {
      password[2] = EnterKey;
      lcd.setCursor(8, 1);
      lcd.print(password[2]);
      delay(500);
      lcd.setCursor(8, 1);
      lcd.print("*");
    }
    if (i == 3) {
      password[3] = EnterKey;
      lcd.setCursor(9, 1);
      lcd.print(password[3]);
      delay(500);
      lcd.setCursor(9, 1);
      lcd.print("*");
    }
 }
             if(on==4) 
                 {
                if(!strcmp(password,On_equip)) // Check if the entered password is to open the door
                  {
                    lcd.clear();
                    lcd.print("    Correct!");
                    myservo.write(100); // Open the doors
                    myservo2.write(180);
                    digitalWrite(RedLed,LOW);
                    delay(1000);
                    lcd.clear();
                    lcd.print("      Opened!");
                    i=0;
                    digitalWrite(ledPin, HIGH); // Turn on the LED
                                  
                  }
                  
                 if(!strcmp(password,Off_equip)) // Check if the entered password is to close the door
                 {
                    lcd.clear();
                    myservo.write(0); 
                    myservo2.write(85);
                    lcd.print("     Closed!");
                    digitalWrite(RedLed,LOW);
                    delay(1000);
                    lcd.clear();
                    lcd.print(" Enter Password");
                    i=0;
                  }
                  
                  if(strcmp(password,On_equip))
                  {
                    if(strcmp(password,Off_equip))
                    {
                    
                    lcd.clear();
                    lcd.print("   Incorrect!");
                    delay(1000);
                    lcd.clear();
                    lcd.print("   Try Again!");
                    delay(1000);
                    lcd.clear();
                    lcd.print(" Enter Password");
                    i = 0;
                    Serial.println(" Wrong password.............");
                    digitalWrite(RedLed,HIGH);
                    
                    }
                  }
                  on=0;
                  }
 }
