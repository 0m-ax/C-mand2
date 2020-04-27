/*
    Ultrasonic sensor Pins:
        VCC: +5VDC
        Trig : Trigger (INPUT) - Pin11
        Echo: Echo (OUTPUT) - Pin 12
        GND: GND
 */
 
int trigPin = 12;    // Trigger
int echoPin = 11;    // Echo
long duration, cm;
struct Buffer {
  int *data;
  int values;
  int len;
  int front; 
};
int data[60];
struct Buffer buffer = {
  data,
  0,
  60,
  0
};


void setup() {
  //Serial Port begin
  Serial.begin (
);
  //Define inputs and outputs
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}
String command = "";
void loop() {
  // send pulse
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
 
  // wait for pulse
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);
  
  // convert the time into a distance
  cm = (duration/2) / 29.1;
  
  // save values in buffer
  *(buffer.data+buffer.front) = cm;
  buffer.front++;
  if(buffer.front > (buffer.len-1)){
    buffer.front = 0;
  }
  buffer.values++;
  if(buffer.values > buffer.len){
    buffer.values = buffer.len;
  }
  
  //proces serial
  if (Serial.available() > 0) {
    command = Serial.readString();
    if(command == "f\n"){
    for(int i = 0; i < buffer.values; i++){
      Serial.print(*(buffer.data+((i+buffer.front)%buffer.values)));
      Serial.print(",");
     }
     Serial.println("");
     buffer.front = 0;
     buffer.values = 0;
    }
  }
  
  delay(250);
}
