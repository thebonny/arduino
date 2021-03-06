#define ROBOT_NAME "RandomBot"

// If you haven't configured your device before use this
#define BLUETOOTH_SPEED 38400 //This is the default baudrate that HC-05 uses
// If you are modifying your existing configuration, use this:
// #define BLUETOOTH_SPEED 57600



/*
  The posible baudrates are:
    AT+UART=1200,0,0 -------1200
    AT+UART=2400,0,0 -------2400
    AT+UART=4800,0,0 -------4800
    AT+UART=9600,0,0 -------9600 - Default for hc-06
    AT+UART=19200,0,0 ------19200
    AT+UART=38400,0,0 ------38400
    AT+UART=57600,0,0 ------57600 - Johnny-five speed
    AT+UART=115200,0,0 -----115200
    AT+UART=230400,0,0 -----230400
    AT+UART=460800,0,0 -----460800
    AT+UART=921600,0,0 -----921600
    AT+UART=1382400,0,0 ----1382400
*/

void setup() {
  pinMode(9, OUTPUT);  // this pin will pull the HC-05 pin 34 (key pin) HIGH to switch module to AT mode
  digitalWrite(9, HIGH);
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  Serial.println("Starting config");
  Serial1.begin(BLUETOOTH_SPEED);
  delay(1000);

  // Should respond with OK
  Serial1.print("AT\r\n");
  waitForResponse();

 
  // Set baudrate to 57600
  Serial1.print("AT+UART=230400,0,0\r\n");
  waitForResponse();

  Serial1.print("AT+UART\r\n");
  waitForResponse();

  Serial.println("Done!");
}

void waitForResponse() {
    delay(1000);
    while (Serial1.available()) {
      Serial.write(Serial1.read());
    }
    Serial.write("\n");
}

void loop() {}
