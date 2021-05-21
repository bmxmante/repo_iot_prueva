#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"
#define DHTPIN 15
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

// defino variables 
const int trigPin = 2;
const int echoPin = 5;

long duration;
int D_ant,Distance;
String stringdistance,str;
//String JSON;
float  h_ant,t_ant;

//**************************************
//*********** MQTT CONFIG **************
//**************************************
const char *mqtt_server = "ioticos.org";
const int   mqtt_port =1883 ;
const char *mqtt_user = "ObR3tRVx5nghNer";
const char *mqtt_pass = "ZqpHVjEV84O48RS";
const char *root_topic_subscribe = "hF39Jx1E462loPz/input";
const char *root_topic_publish = "hF39Jx1E462loPz/output";


//**************************************
//*********** WIFICONFIG ***************
//**************************************
const char* ssid = "BOCADITOS DE AMOUR";
const char* password =  "Kinkbmx.";



//**************************************
//*********** GLOBALES   ***************
//**************************************
WiFiClient espClient;
PubSubClient client(espClient);
char msg[57];
//long count=0;


//************************
//** F U N C I O N E S ***
//************************
void callback(char* topic, byte* payload, unsigned int length);
void reconnect();
void setup_wifi();

  void setup() 
  {
    Serial.begin(9600);
    setup_wifi();
    client.setServer(mqtt_server, mqtt_port); 
    client.setCallback(callback);
    pinMode(trigPin, OUTPUT); // defino como salidad el pin tring del ultrasoido
    pinMode(echoPin, INPUT);  // defino como entrada el pin echo del ultrasoido
    dht.begin();   //inicio el modulo dht11
  }

void loop() {
  
  if (!client.connected()) 
  {
    reconnect();
  }

  if (client.connected())
  {
     // limpia los pines de trigpin 
    digitalWrite(trigPin, LOW);
    delayMicroseconds(10);

    float h = dht.readHumidity();
    float t = dht.readTemperature();

    // coloca el trigpin en alto durante 10 microsegundos
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // lee el pin echo y debuelvo el tiempo de ida y vuelta de la onda en microsegundos
    duration = pulseIn(echoPin, HIGH);

    // Calculo la distancio con esta ecuacion
    Distance = duration * 0.034 / 2;

     str ="Distancia: " + String (Distance) + " Cm"+"Humedad: "+ h +"% Temperatura: "+ t +"°C ";        
    
    // muestro en pantalla las variables
    if(D_ant != Distance || h_ant != h || t_ant != t)
      {

        str.toCharArray(msg,57);
        client.publish(root_topic_publish,msg);
     
//        Serial.print("Distancia: ");
//        Serial.println( String (Distance) + " Cm");
//        Serial.print(F("Humedad: "));
//        Serial.print(h);
//        Serial.print(F("% Temperatura: "));
//        Serial.print(t);
//        Serial.println(F("°C "));
      }
      D_ant=Distance;
      h_ant=h;
      t_ant=t;
      delay(1000);
  }

  //*****************************
  //***    CONEXION WIFI      ***
  //*****************************
  client.loop();
  }
  void setup_wifi()
  {
    delay(10);
    // Nos conectamos a nuestra red Wifi
    Serial.println();
    Serial.print("Conectando a ssid: ");
    Serial.println(ssid);
  
    WiFi.begin(ssid, password);
  
      while (WiFi.status() != WL_CONNECTED)
      {
        delay(500);
        Serial.print(".");
      }
  
    Serial.println("");
    Serial.println("Conectado a red WiFi!");
    Serial.println("Dirección IP: ");
    Serial.println(WiFi.localIP());
}



//*****************************
//***    CONEXION MQTT      ***
//*****************************

void reconnect() {

  while (!client.connected()) {
    Serial.print("Intentando conexión Mqtt...");
    // Creamos un cliente ID
    String clientId = "IOTICOS_H_W_";
    clientId += String(random(0xffff), HEX);
    // Intentamos conectar
      if (client.connect(clientId.c_str(),mqtt_user,mqtt_pass)) 
      {
        Serial.println("Conectado!");
        // Nos suscribimos
        if(client.subscribe(root_topic_subscribe)){
          Serial.println("Suscripcion ok");
        }else
        {
          Serial.println("fallo Suscripciión");
        }
      } 
      else 
      {
      Serial.print("falló :( con error -> ");
      Serial.print(client.state());
      Serial.println(" Intentamos de nuevo en 5 segundos");
      delay(5000);
     }
  }
}


//*****************************   desifra la informacion del brouquer
//***       CALLBACK        ***    en formato char 
//*****************************

void callback(char* topic, byte* payload, unsigned int length){
  String incoming = "";
  Serial.print("Mensaje recibido desde -> ");
  Serial.print(topic);
  Serial.println("");
  for (int i = 0; i < length; i++) {
    incoming += (char)payload[i];
  }
  incoming.trim();
  Serial.println("Mensaje -> " + incoming);

}
