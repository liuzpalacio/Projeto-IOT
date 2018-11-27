#include <eHealth.h>
#include <eHealthDisplay.h>

#include <PinChangeInt.h>
#include <PinChangeIntConfig.h>

#define BT_SENSOR   3
#define LED_SENSOR  2

int cont = 0;
char signal = 'o';
boolean sensorStatus = false;

void setup()
{
  Serial.begin(115200);
  eHealth.initPulsioximeter();

  pinMode(BT_SENSOR, INPUT);
  pinMode(LED_SENSOR, OUTPUT);
  digitalWrite(LED_SENSOR, HIGH);
  delay(2000);
  digitalWrite(LED_SENSOR, LOW);

  //Attach the inttruptions for using the pulsioximeter.
  PCintPort::attachInterrupt(6, readPulsioximeter, RISING);
}

void loop()
{
  if (digitalRead(BT_SENSOR) == HIGH)
  {
    sensorStatus = !sensorStatus;
    digitalWrite(LED_SENSOR, sensorStatus);
    delay(500);
  }

  if (sensorStatus) {
    Serial.println((String)'A' + eHealth.getAirFlow());
    Serial.println((String)'B' + eHealth.getBPM());
    Serial.println((String)'S' + eHealth.getOxygenSaturation());
    delay(500);
  }
}

//Include always this code when using the pulsioximeter sensor
//=========================================================================
void readPulsioximeter()
{
  cont ++;

  if (cont == 50) //Get only of one 50 measures to reduce the latency
  {
    eHealth.readPulsioximeter();
    cont = 0;
  }
}
