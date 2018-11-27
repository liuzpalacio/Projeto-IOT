#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>

//informações da rede WIFI
const char* ssid      = "IEEE1451";        // SSID da rede WIFI
const char* password  = "qpalzm123";           // senha da rede wifi

//informações do broker MQTT - Verifique as informações geradas pelo CloudMQTT
const char* mqttServer        = "192.168.0.4";  // IP do servidor
const int   mqttPort          = 883;            // porta
const char* mqttTopicAirFlow  = "airFlow";
const char* mqttTopicBPM      = "BPM";
const char* mqttTopicSPO2     = "SPO2";

WiFiClient espClient;
PubSubClient client(espClient);

SoftwareSerial mySerial(D8, D9); // RX, TX

void setup()
{
  Serial.begin(115200);
  mySerial.begin(115200);

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

    if (client.connect("ESP8266Client"))
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
}

void callback(char* topic, byte* payload, unsigned int length){}

void reconect()
{
  while (!client.connected())
  {
    Serial.print("Reconectando ao Broker MQTT... ");
    
    if(client.connect("ESP8266ClientTutu"))
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
}

void loop()
{
  if (!client.connected())
  {
    reconect();
  }
  client.loop();

  if(mySerial.available())
  {
    String msg = mySerial.readStringUntil('\n');
    char flag = msg[0];
    String data = msg.substring(1);
    char *p = const_cast<char*>(data.c_str());
    Serial.println((String)flag+p);

    switch (flag)
    {
      case 'A':
        client.publish(mqttTopicAirFlow, p);
        break;
      case 'B':
        client.publish(mqttTopicBPM, p);
        break;
      case 'S':
        client.publish(mqttTopicSPO2, p);
        break;
    }
  }
}
