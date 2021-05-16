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
int D_ant,Distance,bandera,mnum;
int LED = 13; 
int LED1 = 12;
int VENT = 14;  

String stringdistance,str,tem,mens;
float  t,h,h_ant,t_ant;

//**************************************
//*********** MQTT CONFIG **************
//**************************************
const char *mqtt_server = "ioticos.org";
const int   mqtt_port =1883 ;
const char *mqtt_user = "ObR3tRVx5nghNer";
const char *mqtt_pass = "ZqpHVjEV84O48RS";
const char *root_topic_subscribe = "hF39Jx1E462loPz/input";
const char *root_topic_publish = "hF39Jx1E462loPz/output";
const char *root_topic_tem = "hF39Jx1E462loPz/tem";


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
char msgtem[25];
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
    pinMode(LED, OUTPUT);
    pinMode(LED1, OUTPUT);
    pinMode(VENT, OUTPUT);
    digitalWrite(LED, HIGH);
    digitalWrite(LED1, HIGH);
    digitalWrite(VENT, HIGH);
    dht.begin();   //inicio el modulo dht11
  }

void loop() 
{
    
    if (!client.connected())  // si el cliente no se conecta entra
    {
      reconnect();
    }
  
      if (client.connected())
      {
         // limpia los pines de trigpin 
        digitalWrite(trigPin, LOW);
        delayMicroseconds(10);
    
         h = dht.readHumidity();
         t = dht.readTemperature();
    
        // coloca el trigpin en alto durante 10 microsegundos
        digitalWrite(trigPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPin, LOW);
    
        // lee el pin echo y debuelvo el tiempo de ida y vuelta de la onda en microsegundos
        duration = pulseIn(echoPin, HIGH);
    
        // Calculo la distancio con esta ecuacion
        Distance = duration * 0.034 / 2;
    
         str = " Distancia: " + String (Distance) + " Cm "+"Humedad: "+ h +"% Temperatura: "+ t +"°C ";        
         tem = "Temperatura: "+ String (t) +"°C "; 
          // muestro en pantalla las variables
          if(D_ant != Distance || h_ant != h || t_ant != t) //si hay algun cambio muestre en pantalla
            {
             str.toCharArray(msg,57);                 //convierta str en arreclo char con 57 carateres 
             client.publish(root_topic_publish,msg);
             
            //tem.toCharArray(msgtem,25);
            //client.publish(root_topic_tem,msgtem);
            }
          D_ant=Distance;      //guarda el cmabio anterior
          h_ant=h;
          t_ant=t;
          delay(1000);
          casos();
       }
  client.loop();
  
    
}

 //*****************************
  //***    CONEXION WIFI      ***
  //***************************** 
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

void reconnect() 
{

  while (!client.connected()) 
  {
    Serial.print("Intentando conexión Mqtt...");
    // Creamos un cliente ID
    String clientId = "IOTICOS_H_W_";
    clientId += String(random(0xffff), HEX);
    // Intentamos conectar
      if (client.connect(clientId.c_str(),mqtt_user,mqtt_pass)) 
      {
        Serial.println("Conectado!");
        // Nos suscribimos
          if(client.subscribe(root_topic_subscribe))
          {
            Serial.println("Suscripcion ok");
          }
          else
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

  void callback(char* topic, byte* payload, unsigned int length)
  {
    String incoming = "";
    Serial.print("Mensaje recibido desde: ");
    Serial.print(topic);
    Serial.println("");
      for (int i = 0; i < length; i++) 
      {
        incoming += (char)payload[i];
      }
      mens=incoming;                                                //guardo en una variable para poder usarlo  como lectura
    incoming.trim();
    Serial.println("Mensaje: " + incoming);
      casos();                                                    // llama la funcion y dependiendo la orden ejecuta
  
  }
  
   void casos()                                                // funcion "funcion casos"
      {
          if (mens == "a0")
             {
//              digitalWrite(LED1, LOW);
              digitalWrite(LED, HIGH); 
//              digitalWrite(VENT, LOW);
                Serial.println("entro al a0 " ); 
             }
           if (mens == "a1")
             {
               digitalWrite(LED, LOW); 
                Serial.println("entro al a1"); 
             } 
           if (mens == "b0")
             {
                digitalWrite(LED1, HIGH); 
                Serial.println("entro al b0"); 
             } 
           if (mens == "b1")
             {
                digitalWrite(LED1, LOW); 
                Serial.println("entro al b1"); 
             } 
           if (mens == "c0")
             {
                digitalWrite(VENT, HIGH); 
                Serial.println("entro al b0"); 
             } 
           if (mens == "c1")
             {
                digitalWrite(VENT, LOW); 
                Serial.println("entro al b1"); 
             }   
           if (mens[0] == 't' && mens[1] == 'e' && mens[2] == 'm' && mens[3] == ':')
             {
             mnum = ((mens[4]-48)*10) +mens[5]-48;                                 //guardo el numero para usarlo en la temperatura
             Serial.println("entro al tem:");
                  if (t>=mnum) 
                     {
                      digitalWrite(VENT, LOW);          //prendo el ventilaror(trabaja con ceros)
                      Serial.println("entro al mayor que");
                      delay(1000); 
                     } 
                  else  digitalWrite(VENT, HIGH);  
             }  
             
      }  
      
