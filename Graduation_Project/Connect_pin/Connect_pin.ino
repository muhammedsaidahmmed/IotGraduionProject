const int analogPin = A0;  // Analog pin connected to the MQ2 sensor
const float Vcc = 3.3;     // Voltage supplied to the sensor
const float RL = 10;       // Load resistance value in kilo ohms (check your sensor for the correct value)

void setup() {
  Serial.begin(9600);
}

void loop() {
  int sensorValue = analogRead(analogPin);
  float voltage = sensorValue * (Vcc / 1023.0);
  
  float RS = ((Vcc * RL) / voltage) - RL;
  
  // Adjust the values below according to your sensor's calibration
  float ratio = RS / 10.0;  // Ratio of RS/R0, where R0 is the resistance of the sensor in clean air
  float ppm = 10 * pow(ratio, -1.12); // Convert ratio to ppm
  
  Serial.print("Gas concentration (PPM): ");
  Serial.println(ppm);
  
  delay(1000);  // Wait for 1 second before taking the next reading
}