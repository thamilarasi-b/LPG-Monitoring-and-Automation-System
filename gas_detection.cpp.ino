#include <Servo.h>
#include <SoftwareSerial.h>

// Pin Definitions
#define MQ2_PIN A0         // MQ2 Sensor Analog Output
#define BUZZER_PIN 10      // Buzzer Pin
#define SERVO_PIN 9        // Servo Motor PWM Pin
#define SIM800_RX 2        // SIM800L RX Pin
#define SIM800_TX 3        // SIM800L TX Pin

// Threshold for gas leakage detection
#define GAS_THRESHOLD 110  // Adjust based on calibration

// Initialize Servo and SoftwareSerial for SIM800L
Servo myServo;
SoftwareSerial SIM800L(SIM800_RX, SIM800_TX);

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  SIM800L.begin(9600);

  // Initialize GSM module
  initializeGSM();

  // Initialize Servo
  myServo.attach(SERVO_PIN);
  myServo.write(0); // Start position

  // Setup MQ2 Sensor and Buzzer
  pinMode(MQ2_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  Serial.println("System Ready.");
}

void loop() {
  // Call function to check for gas and handle the operations
  checkGasLeakage();

  // Delay before checking again
  delay(1000);
}

// Function that reads gas concentration, activates buzzer and servo, sends SMS, and makes a call
void checkGasLeakage() {
  int gasLevel = analogRead(MQ2_PIN);
  Serial.print("Gas Level: ");
  Serial.println(gasLevel);

  // If gas level exceeds the threshold, trigger actions
  if (gasLevel > GAS_THRESHOLD) {
    Serial.println("Gas leakage detected!");

    // Activate buzzer
    digitalWrite(BUZZER_PIN, HIGH);

    // Rotate servo motor to 180 degrees
    myServo.write(180);
    delay(1000);

    // Send SMS Alert
    sendSMS("Gas leakage detected! Please take immediate action.");
    // Make a call
    makeCall("+91XXXXXXXXX"); // Replace with the phone number

    // Return the servo to the original position after some time
    delay(5000);
    myServo.write(0);

    // Deactivate buzzer
    digitalWrite(BUZZER_PIN, LOW);

    // Prevent continuous SMS spamming and call repetition
    delay(60000);
  }
}

// Initialize GSM module by sending AT commands
void initializeGSM() {
  sendATCommand("AT"); // Check if the module is ready
  sendATCommand("AT+CMGF=1"); // Set SMS to text mode
}

// Function to send AT commands
void sendATCommand(String command) {
  SIM800L.println(command);
  delay(1000);
  while (SIM800L.available()) {
    Serial.write(SIM800L.read()); // Log the response
  }
  Serial.println();
}

// Send SMS using SIM800L
void sendSMS(String message) {
  sendATCommand("AT+CMGS=\"+91XXXXXXXXXX\""); // Replace with the phone number
  delay(1000);
  SIM800L.println(message); // SMS message content
  delay(1000);
  SIM800L.write(26); // ASCII code for CTRL+Z to send the message
  delay(5000);
}

// Make a Call using SIM800L
void makeCall(String phoneNumber) {
  sendATCommand("ATD" + phoneNumber + ";"); // Dial the number
  delay(15000); // Call duration (15 seconds)
  sendATCommand("ATH"); // Hang up the call
  delay(1000);
}
