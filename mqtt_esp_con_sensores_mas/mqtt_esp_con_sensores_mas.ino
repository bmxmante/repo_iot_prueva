#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>                    // incluye libreria de Servo para la placa
#include "DHT.h"                      //incluyo la libretia para el sensor de temperatura
#include "ESP32_MailClient.h"         //incluyo la libreria para enviar correos

#define DHTPIN 15
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);             //creo objeto para el sensor de temperatura
Servo servo1;                         // crea objeto para el servo
// defino variables del sensor ultrasonico
const int trigPin = 2;
const int echoPin = 5;
// defino variables del servomotor
int PINSERVO = 26;                       // pin 34 conectado a señal del servo
int PULSOMIN = 1000;                     // pulso minimo en microsegundos
int PULSOMAX = 2000;                    // pulso maximo en microsegundos

// defino demas variables
long duration;
int D_ant,Distance,bandera,mnum,BANDERA;
int LED = 13;                           //para prender el bombillo 1
int LED1 = 12;                          //para prender el bombillo 2
int VENT = 14;                          //para prender el ventilador
int LED_MAIL = 18;                      //para prender el led indicador que se envio el mensaje
int BOTON = 0;                          //una bandera
int SENSOR = 4;                         //para leer la apertura de la puerta
int mnuman = 0;                         //para almacenas el valor de temperatura naterior en entero
int cont = 500 ;

String stringdistance,str,tem,hum,mens,PUERTA,EMAIL;
float  t,h,h_ant,t_ant;

SMTPData datosSMTP;

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
const char *root_topic_hum = "hF39Jx1E462loPz/hum";


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
char msg[76];
char msgtem[25];
char msghum[25];
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
    servo1.attach(PINSERVO, PULSOMIN, PULSOMAX);  // inicializacion de servo
    pinMode(trigPin, OUTPUT); // defino como salidad el pin tring del ultrasoido
    pinMode(echoPin, INPUT);  // defino como entrada el pin echo del ultrasoido
    pinMode(LED, OUTPUT);
    pinMode(LED1, OUTPUT);
    pinMode(VENT, OUTPUT);
    pinMode(LED_MAIL, OUTPUT);    
    pinMode(SENSOR, INPUT);     //Pulsador o sensor como entrada   
    digitalWrite(LED, HIGH);
    digitalWrite(LED1, HIGH);
    digitalWrite(VENT, HIGH);
    digitalWrite(LED_MAIL, LOW);
    dht.begin();             //inicio el modulo dht11
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
        if(BOTON==1) PUERTA="ABIERTA";
        else PUERTA="CERRADA";
         str = " Distancia: " + String (Distance) + " Cm "+"Humedad: "+ h +"% Temperatura: "+ t +"°C puerta: "+ PUERTA;        
         tem = "Temperatura: "+ String (t) +"°C "; 
         hum = "Humedad: "+ String (h) +"% "; 
          // muestro en pantalla las variables
          if(D_ant != Distance || h_ant != h || t_ant != t) //si hay algun cambio muestre en pantalla
            {
             str.toCharArray(msg,76);                 //convierta str en arreclo char con 57 carateres 
             client.publish(root_topic_publish,msg);  
            }
            if(cont > 500)
                 {
                  tem.toCharArray(msgtem,25);
                  client.publish(root_topic_tem,msgtem);
                  hum.toCharArray(msghum,25);
                  client.publish(root_topic_hum,msghum);
                  cont = 0;
                 }
                else cont++;
          D_ant=Distance;      //guarda el cmabio anterior
          h_ant=h;
          t_ant=t;
          
          BOTON = digitalRead(SENSOR);
          if(BOTON == 1)
            {
            correo();
            }
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
          if (mens == "a0") //if (mens == "cuarto1off")
             {
               digitalWrite(LED, HIGH); 
                Serial.println("entro a apagar cuarto1 " );
             }
           if (mens == "a1") //if (mens == "cuarto1on")
             {
               digitalWrite(LED, LOW); 
                Serial.println("entro a prender cuarto1"); 
             } 
           if (mens == "b0") //if (mens == "cuarto2off")
             {
                digitalWrite(LED1, HIGH); 
                Serial.println("entro a apagar cuarto2"); 
             } 
           if (mens == "b1") //if (mens == "cuarto2on")
             {
                digitalWrite(LED1, LOW); 
                Serial.println("entro a prender cuarto2"); 
             } 
           if (mens == "c0") //if (mens == "ventiladoroff")
             {
                digitalWrite(VENT, HIGH); 
                Serial.println("entro a apagar ventilador"); 
             } 
           if (mens == "c1")  //if (mens == "ventiladoron")
             {
                digitalWrite(VENT, LOW); 
                Serial.println("entro a prender ventilador"); 
             }   
           if (mens == "d0")  //if (mens == "cerrar")
             {
                servo1.write(0);    // ubica el servo a 0 grados
                delay(5000);  
                Serial.println("entro a cerrar puerta"); 
             } 
           if (mens == "d1") //if (mens == "abrir")
             {  
                servo1.write(180);    // ubica el servo a 0 grados
                //correo();
                delay(5000); 
                Serial.println("entro a abrir puerta"); 
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
                  
                if(mnum != mnuman)
                {
                  BANDERA = 1;       //si cambio la tempratura
                  correo(); 
                  BANDERA = 0; 
                }
                mnuman = mnum;                       //para que no entre si no cuando la temperatura cambie             
                                         
             }  
             
      }  


       void correo()
  { 
    Serial.print("Iniciando correo!!!");
    delay(1000);
    EMAIL="SE ACABA DE ABRIR LA PUERTA"; 
     if(BANDERA==1) EMAIL="CAMBIO LA TEMPERATURA";          //este if es para cuando se esta cambiando la temperaturo y no abriendo la puerta
     digitalWrite(LED_MAIL, HIGH);                         //prendo el led indicando que se esta enviendo un email
    datosSMTP.setLogin("smtp.gmail.com", 465, "labiot2021@gmail.com", "laboratorio2021");//Configuración del servidor de correo electrónico SMTP, host, puerto, cuenta y contraseña
    datosSMTP.setSender("DANIEL_ESP32", "labiot2021@gmail.com");     //coloca el nombre del que envia el mensaje y el correo electrónico
    datosSMTP.setPriority("High");                            // Establezca la prioridad o importancia del correo electrónico High, Normal, Low o 1 a 5 (1 es el más alto)
    datosSMTP.setSubject("ALERTA DE SEGURIDAD");               // Establecer el asunto
    datosSMTP.setMessage(EMAIL, false);                       // escribe el contenido del correo
    datosSMTP.addRecipient("danielrincon@unisangil.edu.co");   // aqui se envia el correo al destinatario     
    if (!MailClient.sendMail(datosSMTP))                       //si tiene datos comience a enviar correo electrónico.
    Serial.println("Error enviando el correo, " + MailClient.smtpErrorReason());
    datosSMTP.empty();           //Borrar todos los datos del objeto datosSMTP para liberar memoria
      delay(9000);                 // esperao 9min a que se envie el mensaje
     digitalWrite(LED_MAIL, LOW);    //apago led despues de enviar el mensaje
     BANDERA==0;
  } 
