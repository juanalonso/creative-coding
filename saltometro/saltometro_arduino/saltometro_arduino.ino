// Board: LOLIN D32
// OJO, en boards manager, hay que elegir la versión 2.0.11 de "ESP32 by Espressif"
// Hay que conectar el pin SDA del LOLIN con el SDA del acelerómetro. Lo mismo para el pin SCL.

#include <Adafruit_LSM303_Accel.h>
#include <Adafruit_Sensor.h>
#include <BleKeyboard.h>

#define THRESHOLD_JUMP_START 5.0  // Umbral para detectar el inicio del salto (en g)
#define THRESHOLD_JUMP_LAND 15.0  // Umbral para detectar el aterrizaje (en g)
#define DEBOUNCE 150UL            // Tiempo de debounce en ms
#define WINDOW_SIZE 5             // Tamaño de la ventana para el filtro
#define MIN_JUMP_DURATION 100     // Duración mínima del salto en ms
#define MAX_JUMP_DURATION 500     // Duración máxima del salto en ms

#define DEBUG
// #define PASADIAPOS

Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(54321);
BleKeyboard bleKeyboard("Saltometro", "Kokuma", 80);

float accWindow[WINDOW_SIZE];
int windowIndex = 0;

float filteredAcc = 0;
bool inJump = false;
unsigned long jumpStartTime = 0;
unsigned long lastJumpTime = 0;

float calculateMovingAverage() {
  float sum = 0;
  for (int i = 0; i < WINDOW_SIZE; i++) {
    sum += accWindow[i];
  }
  return sum / WINDOW_SIZE;
}

float calculateAccelMagnitude(sensors_event_t &event) {
  float x = event.acceleration.x;
  float y = event.acceleration.y;
  float z = event.acceleration.z;
  return sqrt(x * x + y * y + z * z);
}

void setup(void) {

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  delay(2000);

  Serial.begin(115200);
  Serial.println("Saltometro 2024");
  Serial.println("");

  if (!accel.begin()) {
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("Ooops, no LSM303 detected ... Check your wiring!");
    while (1)
      ;
  }

  accel.setRange(LSM303_RANGE_16G);

  bleKeyboard.begin();

  sensors_event_t event;
  accel.getEvent(&event);
  float initialMag = calculateAccelMagnitude(event);
  for (int i = 0; i < WINDOW_SIZE; i++) {
    accWindow[i] = initialMag;
  }
}

void loop(void) {

  sensors_event_t event;
  accel.getEvent(&event);

  float currentMag = calculateAccelMagnitude(event);
  accWindow[windowIndex] = currentMag;
  windowIndex = (windowIndex + 1) % WINDOW_SIZE;
  filteredAcc = calculateMovingAverage();

  unsigned long currentTime = millis();

  if (!inJump && filteredAcc < THRESHOLD_JUMP_START && (currentTime - lastJumpTime) > DEBOUNCE) {
    inJump = true;
    jumpStartTime = currentTime;
    digitalWrite(LED_BUILTIN, LOW);

#ifdef DEBUG
    Serial.print("Acc INICIO: ");
    Serial.println(filteredAcc);
#endif
  }

  if (inJump && filteredAcc > THRESHOLD_JUMP_LAND) {
    unsigned long jumpDuration = currentTime - jumpStartTime;
    if (jumpDuration > MIN_JUMP_DURATION && jumpDuration < MAX_JUMP_DURATION) {
      if (bleKeyboard.isConnected()) {
#ifdef PASADIAPOS
        bleKeyboard.write(KEY_RIGHT_ARROW);
#else
        bleKeyboard.print('1');
#endif

#ifdef DEBUG
        Serial.print("Acc FIN: ");
        Serial.print(filteredAcc);
        Serial.print(" - ");
        Serial.print(jumpDuration);
        Serial.println(" ms");
#endif
      }
      lastJumpTime = currentTime;
    }
#ifdef DEBUG
    else {
      Serial.print("Descartado por duración inválida - ");
      Serial.print(jumpDuration);
      Serial.println(" ms");
    }
#endif

    inJump = false;
    digitalWrite(LED_BUILTIN, HIGH);
  }

  // Serial.print(0);
  // Serial.print(" ");
  // Serial.print(50);
  // Serial.print(" ");
  // Serial.print(currentMag);
  // Serial.print(" ");
  // Serial.println(filteredAcc);



  delay(20);
}


void displaySensorDetails(void) {
  sensor_t sensor;
  accel.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print("Sensor:       ");
  Serial.println(sensor.name);
  Serial.print("Driver Ver:   ");
  Serial.println(sensor.version);
  Serial.print("Unique ID:    ");
  Serial.println(sensor.sensor_id);
  Serial.print("Max Value:    ");
  Serial.print(sensor.max_value);
  Serial.println(" m/s^2");
  Serial.print("Min Value:    ");
  Serial.print(sensor.min_value);
  Serial.println(" m/s^2");
  Serial.print("Resolution:   ");
  Serial.print(sensor.resolution);
  Serial.println(" m/s^2");
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}