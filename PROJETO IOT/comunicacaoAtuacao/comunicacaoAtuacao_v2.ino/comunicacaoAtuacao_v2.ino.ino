#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>


#define LED_AIRFLOW   14
#define LED_BPM       13
#define LED_SPO2      12

#define LED_ACTUADOR  4
#define BT_ACTUADOR   15

//informações da rede WIFI
const char* ssid      = "IEEE1451";        // SSID da rede WIFI
const char* password  = "qpalzm123";           // senha da rede wifi

//informações do broker MQTT - Verifique as informações geradas pelo CloudMQTT
const char* mqttServer        = "192.168.0.4";  // IP do servidor
const int   mqttPort          = 883;            // porta
const char* mqttTopicAirFlow  = "airFlowFlag";
const char* mqttTopicBPM      = "BPMFlag";
const char* mqttTopicSPO2     = "SPO2Flag";

boolean actuadorStatus = false;

WiFiClient espClient;
PubSubClient client(espClient);


void setup()
{
  Serial.begin(115200);

  pinMode(LED_AIRFLOW,  OUTPUT);
  pinMode(LED_BPM,      OUTPUT);
  pinMode(LED_SPO2,     OUTPUT);

  pinMode(LED_ACTUADOR, OUTPUT);
  pinMode(BT_ACTUADOR,  INPUT);

  digitalWrite(LED_ACTUADOR, actuadorStatus);

  digitalWrite(LED_AIRFLOW, HIGH);
  digitalWrite(LED_BPM, HIGH);
  digitalWrite(LED_SPO2, HIGH);

  delay(2000);

  digitalWrite(LED_AIRFLOW, LOW);
  digitalWrite(LED_BPM, LOW);
  digitalWrite(LED_SPO2, LOW);


  //set the ESP8266 to be a WiFi-client, otherwise,
  //could cause network-issues with your other WiFi-devices
  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid, password);
  Serial.print("\n\nConnecting to ");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  //conecta ao servidor MQTT e atribui função de callblac
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  while (!client.connected())
  {
    Serial.print("Conectando ao Broker MQTT... ");

    if (client.connect("Actuator"))
    {
      Serial.println("Conectado");
    }
    else
    {
      Serial.print("FALHA: ");
      Serial.println(client.state());
      delay(2000);
    }
  }


  client.subscribe(mqttTopicBPM);
  client.subscribe(mqttTopicSPO2);
  client.subscribe(mqttTopicAirFlow);

}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println(topic);

  if (strcmp(topic, mqttTopicBPM) == 0) {
    if ((char)payload[0] == '1')
    {
      digitalWrite(LED_BPM, HIGH);
    }
    else if ((char)payload[0] == '0')
    {
      digitalWrite(LED_BPM, LOW);
    }
  }

  if (strcmp(topic, mqttTopicSPO2) == 0) {
    if ((char)payload[0] == '1')
    {
      digitalWrite(LED_SPO2, HIGH);
    }
    else if ((char)payload[0] == '0')
    {
      digitalWrite(LED_SPO2, LOW);
    }
  }

  if (strcmp(topic, mqttTopicAirFlow) == 0) {

    if ((char)payload[0] == '1') {
      digitalWrite(LED_AIRFLOW, HIGH);
    }
    else if ((char)payload[0] == '0')
    {
      digitalWrite(LED_AIRFLOW, LOW);
    }
  }

}

void reconect() {
  while (!client.connected()) {
    Serial.print("Reconectando ao Broker MQTT... ");

    if (client.connect("Actuator"))
    {
      Serial.println("Conectado!");
    }
    else
    {
      Serial.print("FALHA: ");
      Serial.println( String(client.state()).c_str());
      Serial.println("Tentando novamente em 10s");
      delay(10000);
    }
  }

  client.subscribe(mqttTopicBPM);
  client.subscribe(mqttTopicSPO2);
  client.subscribe(mqttTopicAirFlow);
}

void loop() {

  if (!client.connected()) {
    reconect();
  }
  client.loop();

  if (digitalRead(BT_ACTUADOR) == HIGH) {
    Serial.println("apertou o botão");
    actuadorStatus = !actuadorStatus;

    digitalWrite(LED_ACTUADOR, actuadorStatus);

    if (!actuadorStatus) {
      digitalWrite(LED_AIRFLOW, LOW);
      digitalWrite(LED_BPM, LOW);
      digitalWrite(LED_SPO2, LOW);
    }

    delay(500);
  }

}
