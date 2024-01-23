#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL6xZdgFDjS"
#define BLYNK_TEMPLATE_NAME "Smart Home"
#define BLYNK_AUTH_TOKEN "VLpgDSQBjzJuqFfnInHRY16NMYXj_zIM"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Blynk authentication token
char auth[] = BLYNK_AUTH_TOKEN;

// WiFi credentials
char ssid[] = "THE LEE";
char pass[] = "leehotel05";

// Define GPIO pins
#define DHTPIN 13       // Safe GPIO for DHT11 sensor
#define LED1PIN 4       // GPIO for LED 1
#define LED2PIN 5       // GPIO for LED 2
#define BUTTON1PIN 14   // GPIO for Button 1
#define BUTTON2PIN 15   // GPIO for Button 2
#define I2C_SDA 21      // SDA pin for I2C communication
#define I2C_SCL 22      // SCL pin for I2C communication

// DHT11 sensor setup
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// LCD setup with I2C
LiquidCrystal_I2C lcd(0x27, 16, 2); // Set the LCD I2C address

// Variables to store the state of the LEDs
bool led1State = LOW;
bool led2State = LOW;

BlynkTimer timer;

void sendSensorData() {
    // Read humidity and temperature from DHT11
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    if (!isnan(humidity) && !isnan(temperature)) {
        // Send integer data to Blynk
        Blynk.virtualWrite(V2, int(temperature));
        Blynk.virtualWrite(V3, int(humidity));

        // Update LCD with the sensor values
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Welcome home!");
        lcd.setCursor(0, 1);
        lcd.print("T:");
        lcd.print(int(temperature));
        lcd.print("*C & H:");
        lcd.print(int(humidity));
        lcd.print("%");
    } else {
        // Display an error message on the LCD if sensor data is invalid
        lcd.setCursor(0, 1);
        lcd.print("Sensor error");
    }
}

// Function to update LED state and send it to Blynk
void updateLEDState(int ledPin, bool &ledState, int virtualPin) {
    ledState = !ledState;
    digitalWrite(ledPin, ledState);
    Blynk.virtualWrite(virtualPin, ledState);
}

// Function to check physical buttons for input
void checkPhysicalButtons() {
    static unsigned long lastButtonPressTime1 = 0;
    static unsigned long lastButtonPressTime2 = 0;
    const unsigned long debounceTime = 200; // Debounce time in milliseconds

    unsigned long currentTime = millis();

    if (currentTime - lastButtonPressTime1 > debounceTime) {
        if (digitalRead(BUTTON1PIN) == LOW) {
            Serial.println("Button 1 pressed");
            updateLEDState(LED1PIN, led1State, V0);
            lastButtonPressTime1 = currentTime; // Update the last button press time for button 1
        }
    }

    if (currentTime - lastButtonPressTime2 > debounceTime) {
        if (digitalRead(BUTTON2PIN) == LOW) {
            Serial.println("Button 2 pressed");
            updateLEDState(LED2PIN, led2State, V1);
            lastButtonPressTime2 = currentTime; // Update the last button press time for button 2
        }
    }
}


// Blynk function to update LED state when controlled from the app
BLYNK_WRITE(V0) {
    led1State = param.asInt();
    digitalWrite(LED1PIN, led1State);
}

BLYNK_WRITE(V1) {
    led2State = param.asInt();
    digitalWrite(LED2PIN, led2State);
}



void setup() {
    Serial.begin(9600);
    pinMode(LED1PIN, OUTPUT);
    pinMode(LED2PIN, OUTPUT);
    pinMode(BUTTON1PIN, INPUT_PULLUP);
    pinMode(BUTTON2PIN, INPUT_PULLUP);
    dht.begin();
    Wire.begin(I2C_SDA, I2C_SCL);
    lcd.init();
    lcd.backlight();
    WiFi.begin(ssid, pass);
    Blynk.begin(auth, ssid, pass); // Initialize Blynk with authentication token
    timer.setInterval(2000L, sendSensorData); // Schedule sensor data update every 2 seconds
    timer.setInterval(100L, checkPhysicalButtons); // Check physical buttons every 100 milliseconds
}

void loop() {
    Blynk.run(); // Handle Blynk events
    timer.run(); // Execute scheduled tasks
}
