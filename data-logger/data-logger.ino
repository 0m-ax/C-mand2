/*
    Ultrasonic sensor Pins:
        VCC: +5VDC
        Trig : Trigger (INPUT) - Pin11
        Echo: Echo (OUTPUT) - Pin 12
        GND: GND
*/
int trigPin = 12;    // Trigger
int echoPin = 11;    // Echo

//buffer stuff
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


struct Sensor {
  long (*func)();
  char *name;
};

long readDistanceSensor() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // wait for pulse
  pinMode(echoPin, INPUT);
  long duration = pulseIn(echoPin, HIGH);

  // convert the time into a distance
  long cm = (duration / 2) / 29.1;
  return cm;
}
struct Sensor distanceSensor = {
  &readDistanceSensor,
  "Distance Sensor"
};

long readDummySensor() {
  return random(0, 10000);
}
struct Sensor dummySensor = {
  &readDummySensor,
  "Dummy Sensor"
};



Sensor sensors[2] = {distanceSensor, dummySensor};

String command = "";

//program state {
Sensor *currentSensor = &dummySensor;

bool logging = false;
long last_update = 0;
long update_interval = 250;

// }

void printMenu() {
  Serial.println("==MENU==");
  Serial.println("s: start/stop recording");
  Serial.println("i: info on current config");
  Serial.println("f: flush and print values");
  Serial.println("p: change sensor");

}

void setup() {
  //Serial Port begin
  Serial.begin(115200);
  //setup distance sensor
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  printMenu();
}
void mainMenu(char *data){
    return "sensor"
}

void loop() {
  if (logging && last_update < millis() - update_interval) {
    last_update = millis();
    long value = (*currentSensor->func)();
    // save values in buffer
    *(buffer.data + buffer.front) = value;
    buffer.front++;
    if (buffer.front > (buffer.len - 1)) {
      buffer.front = 0;
    }
    buffer.values++;
    if (buffer.values > buffer.len) {
      buffer.values = buffer.len;
    }
  }
  //proces serial
  if (Serial.available() > 0) {
    command = Serial.readString();
    if (command == "f\n") {
      Serial.print("values(");
      Serial.print(buffer.values);
      Serial.print("):");
      for (int i = 0; i < buffer.values; i++) {
        Serial.print(*(buffer.data + ((i + buffer.front) % buffer.values)));
        if (i != buffer.values - 1) {
          Serial.print(",");
        }
      }
      Serial.println("");
      buffer.front = 0;
      buffer.values = 0;
    } else if (command == "s\n") {
      logging = !logging;
      if (logging) {
        Serial.println("started logging");
      } else {
        Serial.println("stopped logging");
      }
    } else if (command == "i\n") {
      Serial.print("the current sensor is: ");
      Serial.println((*currentSensor).name);
      Serial.print("the update interval is: ");
      Serial.println(update_interval);

    } else if (command == "d\n") {
      Serial.println("enter update interval");
      // block and wait for user input
      while (Serial.available() == 0 ) {
        //spin me right round
      }
      update_interval = Serial.parseInt();
      Serial.readString();
    } else if (command == "p\n") {
      for (int i = 0; i < sizeof(sensors)/sizeof(sensors[0]); i++) {
        Serial.print(i);
        Serial.print(" : ");
        Serial.println(sensors[i].name);
      }
      Serial.println("enter sensor:");
      // block and wait for user input
      while (Serial.available() == 0 ) {
        //spin me right round
      }
      int sensor_index = Serial.parseInt();
      if(sensor_index >= 0 && sensor_index <sizeof(sensors)/sizeof(sensors[0])){
        currentSensor = &sensors[sensor_index];
      }else{
        Serial.println("unkown sensor");
      }
      Serial.readString();
    } else {
      Serial.print("unknown command:");
      Serial.println(command);
    }
    printMenu();
  }
  //TO DO: turn into FSM, make Menu remember new commands

}
