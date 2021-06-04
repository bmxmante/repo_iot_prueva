#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>                    // incluye libreria de Servo para la placa
#include "DHT.h"                      //incluyo la libretia para el sensor de temperatura
#include "ESP32_MailClient.h"         //incluyo la libreria para enviar correos
                                                  #include "ThingSpeak.h" 

#define DHTPIN 15
#define DHTTYPE DHT11
#define LED_BUILTIN 2

DHT dht(DHTPIN, DHTTYPE);             //creo objeto para el sensor de temperatura
Servo servo1;
Servo servo2;                          // crea objeto para el servo

unsigned long channelID = 1404814;
const char * WriteAPIKey = "JWQZ5YKE6EPNEGGJ";
unsigned long tiempo = 0;
unsigned long tiempo2 = 0;
int periodo = 20000;

// defino variables del sensor ultrasonico
const int trigPin = 2;               // son las entradas para el sensor de distancia
const int echoPin = 5;

// defino variables del servomotor
int PINSERVO = 25;                       // pin 26 conectado a señal del servo
int PULSOMIN = 900;                     // pulso minimo en microsegundos
int PULSOMAX = 2000;                    // pulso maximo en microsegundos

int PINSERVO2 = 26;
int PULSOMIN2 = 900;                     // pulso minimo en microsegundos
int PULSOMAX2 = 2000;                    // pulso maximo en microsegundos

float duracion;
float ant_duracion = 400; 
double Distancia;
const int mbomba = 27;
float prueva;

// defino demas variables
int D_ant,bandera;
int BANDERA = 1;
int mnum =100;
int BOMBILLOS = 12;                          //para prender el bombillo 2
int ALARMA = 13;                           //para prender el bombillo 1
int VENT = 14;                          //para prender el ventilador
int MOTOBOMBA = 27; 

int LED_MAIL = 18;                      //para prender el led indicador que se envio el mensaje
int BOTON ;                          //una bandera
int SENSOR = 4;                         //para leer la apertura de la puerta
int mnuman = 0;                         //para almacenas el valor de temperatura aterior en entero
int cont = 500 ;


int BANDERA_TEM=0;
int BANDERAS1 = 0;
int BANDERAS2 = 0;
int BANDERA_A = 0;
int BANDERA_B = 0;
int BANDERA_V = 0;
int BANDERA_M = 0;
int BANDERA_MA = 0;
int BANDERA_T = 0;
int BANDERA_PCERRADA = 0;
int porcentaje_t = 0;

String stringDistancia,str,tem,hum,mens,PUERTA,EMAIL;
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
const char *root_topic_tempubli = "hF39Jx1E462loPz/tempubli";
const char *root_topic_hum = "hF39Jx1E462loPz/hum";
const char *root_topic_humpubli = "hF39Jx1E462loPz/humpubli";


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
  { mens == "empezo" ;
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(9600);
    setup_wifi();
     ThingSpeak.begin(espClient);
    client.setServer(mqtt_server, mqtt_port); 
    client.setCallback(callback);
    servo1.attach(PINSERVO, PULSOMIN, PULSOMAX);  // inicializacion de servo
    servo2.attach(PINSERVO2, PULSOMIN2, PULSOMAX2);  // inicializacion de servo
    pinMode(trigPin, OUTPUT); // defino como salidad el pin tring del ultrasoido
    pinMode(echoPin, INPUT);  // defino como entrada el pin echo del ultrasoido
    
    pinMode(mbomba , OUTPUT);  // Salida de la electrobomba
    pinMode(ALARMA, OUTPUT);
    pinMode(BOMBILLOS, OUTPUT);
    pinMode(VENT, OUTPUT);
    pinMode(MOTOBOMBA, OUTPUT);
    
    pinMode(BOTON, OUTPUT);
    pinMode(LED_MAIL, OUTPUT);    
    pinMode(SENSOR, INPUT);          //Pulsador o sensor como entrada  
     
    digitalWrite(ALARMA, HIGH);     //les doy un alto iniciando para que no se prendan los relay ya que prenden con ceros
    digitalWrite(BOMBILLOS, HIGH);
    digitalWrite(VENT, HIGH);
    digitalWrite(MOTOBOMBA, HIGH);

    digitalWrite(LED_MAIL, LOW);
    dht.begin();                        //inicio el modulo dht11
  }

void loop() 
{
     
    if (!client.connected())         // si el cliente no se conecta entra
    {
      reconnect();
    }
  
      if (client.connected())
      {  
         h = dht.readHumidity();            //leo la humedad del sensor y lo guardo en h
         t = dht.readTemperature();         //leo la temperatura del sensor y lo guardo en t
         
          tiempo = millis(); 
         if(tiempo > tiempo2 + periodo)
         {   
             ThingSpeak.setField(1,t);
             ThingSpeak.setField(2,h);
             ThingSpeak.writeFields(channelID , WriteAPIKey);
             tiempo2 = tiempo;
         }
          
          digitalWrite(trigPin, LOW);
          delayMicroseconds(2);                                          
          digitalWrite(trigPin, HIGH);// coloca el trigpin en alto durante 10 microsegundos
          delayMicroseconds(10);
          digitalWrite(trigPin, LOW);
      
          // lee el pin echo y debuelvo el tiempo de ida y vuelta de la onda en microsegundos
          duracion = pulseIn(echoPin, HIGH);
          prueva = ant_duracion - duracion;
          if(prueva < 0) prueva = prueva*-1;  
            if(prueva <= 60)
              {
              // Calculating the Distancia
              Distancia = duracion * 0.034 / 2;
              }
           ant_duracion = duracion;
           
          if(BANDERA_MA == 1)sensor_bomba();
          if(BANDERA_T == 1 || t >= mnum )temperatura(); 
          if(t >= 35)
          {   
              digitalWrite(VENT, LOW);
              digitalWrite(ALARMA, LOW);
              delay(500);
              digitalWrite(ALARMA, HIGH);
              delay(500);
              BANDERA_T = 1;                 //bandera para indicar que se subio mucho la temperatura y no apaga el ventilador hasta que baje la tempareatura
          }
          if(t < 30 &&  BANDERA_T == 1 && BANDERA_V == 0)
            {
              digitalWrite(VENT, HIGH);
              BANDERA_T = 0;
            }
            BOTON = digitalRead(SENSOR);
            if(mens != "s11"  && BOTON == 0 && BANDERA_PCERRADA == 1) 
                {
                  while( mens != "des")
                      {
                      BOTON = digitalRead(SENSOR);
                       if (BANDERA == 1)correo(); 
                      digitalWrite(ALARMA, LOW);
                      delay(100);
                      digitalWrite(ALARMA, HIGH);
                      delay(100);
                      }
                }
          if(BOTON == 0) PUERTA ="ABIERTA";       // son para escribir si esta abierta o cerrada la puerta segun el caso
          else PUERTA ="CERRADA";
        
         str = " Ntanque: " + String (porcentaje_t) + "% Temperatura: "+ t +"°C puerta: "+ PUERTA;        
         //tem = "tem:"+ String (t) +"°C "; 
         //hum = "Humedad: "+ String (h) +"% "; 
          // muestro en pantalla las variables
          
          if(D_ant != Distancia || h_ant != h || t_ant != t) //si hay algun cambio muestre en pantalla
            {
             str.toCharArray(msg,76);                 //convierta str en arreclo char con 57 carateres 
             client.publish(root_topic_publish,msg);  
            }
            
          D_ant=Distancia;      //guarda el cambio anterior para la funcion si cambia la variables muestra
          h_ant=h;
          t_ant=t; 

          BOTON = digitalRead(SENSOR);  //lee si esta abierta la puerta
          if(BOTON == 0 && BANDERAS1 == 0)
            {
              
            if (BANDERA == 1)correo();                //se llama a correo para decir que esta abierta la puerta
            Serial.print("se abrio la puerta con el boton IF con el boton  \n ");
            BANDERAS1 = 1;
            }
           if(BOTON == 1 && BANDERAS1 == 1)
           {
            Serial.print("se abrio la puerta con el boton ELSE \n ");
            BANDERAS1 = 0;
           }

          casos();
       }
  client.loop();
  
    
}
  void sensor_bomba()
 {
     Serial.print("Distancia:  ");
     Serial.println( String (Distancia) + " Cm    "+ String (prueva) + " = prueva ");

        if(Distancia >= 8) {
               porcentaje_t= 10;
              digitalWrite(mbomba, LOW);
              Serial.println( " entro a prender motor"); 
          }
            if(Distancia >= 7 && Distancia < 7.5)   {
              porcentaje_t= 16;
          }        
           if(Distancia >= 6.5 && Distancia < 7)   {
              porcentaje_t= 28;
          } 
            if(Distancia >= 6 && Distancia < 6.5)   {
              porcentaje_t= 40;
          } 
            if(Distancia >= 5.5 && Distancia < 6)  {
              porcentaje_t= 52;
          }  
          if(Distancia >= 5 && Distancia < 5.5)   {
              porcentaje_t= 64;
          } 
            if(Distancia >= 4.5 && Distancia < 5)   {
              porcentaje_t= 76;
          } 
            if(Distancia >= 4 && Distancia < 4.5)  {
              porcentaje_t= 88;
          }                  
        if(Distancia < 3.8)  {
              porcentaje_t= 100;
              digitalWrite(mbomba, HIGH);
              Serial.println( " entro a apagar motor");
          }   
 }
  
   void casos()                                                // funcion "funcion casos"
      { 
          //estos if son para prender y apagar el bombillo de la abitacion # 1
          if (mens == "a0" && BANDERA_A == 1) 
             {
               digitalWrite(ALARMA, HIGH); 
               BANDERA_A = 0;
                Serial.println("entro a apagar alarma " );
             }
           if (mens == "a1" && BANDERA_A == 0) 
             {
               digitalWrite(ALARMA, LOW);
               BANDERA_A = 1; 
                Serial.println("entro a prender alarma"); 
             } 
           //estos if son para prender y apagar el bombillo de la abitacion # 2  
           if (mens == "b0" && BANDERA_B == 1) 
             {
                digitalWrite(BOMBILLOS, HIGH);
                BANDERA_B = 0; 
                Serial.println("entro a apagar bombillos"); 
             } 
           if (mens == "b1" && BANDERA_B == 0) 
             {
                digitalWrite(BOMBILLOS, LOW); 
                BANDERA_B = 1;
               Serial.println("entro a prender bombillos"); 
             } 
           //estos if son para prender y apagar el ventilador  
           if (mens == "v0" && BANDERA_V == 1) 
             {
                digitalWrite(VENT, HIGH); 
                BANDERA_V = 0;
                Serial.println("entro a apagar ventilador"); 
             } 
           if (mens == "v1" && BANDERA_V == 0)  //if (mens == "ventiladoron")
             {
                digitalWrite(VENT, LOW); 
                BANDERA_V = 1;
                Serial.println("entro a prender ventilador"); 
             } 
            if (mens == "m0" && BANDERA_M == 1) 
             {
                digitalWrite(MOTOBOMBA, HIGH); 
                BANDERA_M = 0;
                Serial.println("entro a apagar motobomba"); 
             } 
           if (mens == "m1" && BANDERA_M == 0)  //if (mens == "ventiladoron")
             {
                digitalWrite(MOTOBOMBA, LOW); 
                BANDERA_M = 1;
                Serial.println("entro a prender motobomba"); 
             } 
            if (mens == "ma0" && BANDERA_MA == 1) 
             {
                digitalWrite(MOTOBOMBA, HIGH); 
                BANDERA_MA = 0;
                Serial.println("entro a apagar motobomba atomatico"); 
             } 
           if (mens == "ma1" && BANDERA_MA == 0)  //if (mens == "ventiladoron")
             {
                sensor_bomba();
                BANDERA_MA = 1;
                Serial.println("entro a prender motobomba automatico"); 
             } 


            //estos if son para cerrar o abrir la puerta principal de la casa   
           if (mens == "s10" )                        //para cerrar la puerta
             {
                BOTON = digitalRead(SENSOR);            
                if(BOTON == 0)
                  {
                  servo1.write(0);                // ubica el servo a 0 grados
                  delay(2000);   
                  Serial.println("entro a cerrar puerta garaje"); 
                  BANDERA_PCERRADA = 1;
                  }
                
             } 
             
           if (mens == "s11" )                        //para abrir la puerta
             {  
              BOTON = digitalRead(SENSOR);
              if(BOTON == 1)
                {
                servo1.write(180);    // ubica el servo a 0 grados
                Serial.println("entro a abrir puerta garaje"); 
                delay(500); 
                }
             }    
           //estos if son para cerrar o abrir la puerta principal de la casa   
           if (mens == "s20" && BANDERAS2 == 1)  
             {
                servo2.write(180);    // ubica el servo a 0 grados
                delay(300);   
                BANDERAS2 = 0;
                Serial.println("entro a cerrar puerta habitacion"); 
             } 
             
           if (mens == "s21" && BANDERAS2 == 0)
             {  
                servo2.write(0);    // ubica el servo a 0 grados
                if (BANDERA == 0)correo();
                  delay(300);
                  BANDERAS2 = 1;
                  Serial.println("entro a abrir puerta habitacion"); 
             } 
     
             
           //este if es para cuando el cliente desea poner una temeratura para que se prende el ventilador
           if (mens[0] == 't' && mens[1] == 'e' && mens[2] == 'm' && mens[3] == ':')
             {  
              mnum = ((mens[4]-48)*10) +mens[5]-48;                //guardo el numero para usarlo en la temperatura
              temperatura();   
             }  
             
      }  
      
     void temperatura()
     {             
              if (mnum >= t)   digitalWrite(VENT, HIGH); 
              else digitalWrite(VENT, LOW); 

     }
      


       void correo()
  { 
    Serial.print("Iniciando correo!!!");
    EMAIL="ALERTA SE A ABIERTO LA PUERTA"; 
     if(BANDERA_T == 1) EMAIL="ALERTA ALTA TEMPERATURA";          //este if es para cuando se esta cambiando la temperaturo y no abriendo la puerta
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
     digitalWrite(LED_MAIL, LOW);    //apago led despues de enviar el mensaje
     BANDERA++;
  } 


  
 //*****************************
  //***    CONEXION WIFI      ***
  //***************************** 
  void setup_wifi()
  {
    delay(10);
    // Nos conectamos a nuestra red Wifi
    Serial.println();
    Serial.print("Conectando a red Wifi: ");
    digitalWrite(mbomba, HIGH);
    Serial.println(ssid);
    WiFi.begin(ssid, password);
  
      while (WiFi.status() != WL_CONNECTED)
      { 
        digitalWrite(LED_BUILTIN, HIGH);
        delay(250);
        digitalWrite(LED_BUILTIN, LOW);
        delay(250);
        Serial.print(".");
      }
  
    Serial.println("");
    Serial.println("Conectado a red WiFi!");
    Serial.println("Dirección IP: ");
    Serial.println(WiFi.localIP());
    if(WiFi.status() != WL_CONNECTED) digitalWrite(LED_BUILTIN, LOW);
    else digitalWrite(LED_BUILTIN, HIGH);
  }



//*****************************
//***    CONEXION MQTT      ***
//*****************************

void reconnect() 
{

  while (!client.connected()) 
  {
    Serial.print("Intentando conexión Mqtt...");
    digitalWrite(mbomba, HIGH);
    // Creamos un cliente ID
    String clientId = "IOTICOS_H_W_";
    clientId += String(random(0xffff), HEX);
    // Intentamos conectar
      if (client.connect(clientId.c_str(),mqtt_user,mqtt_pass)) //aqui
      {
        Serial.println("Conectado!");
        // Nos suscribimos
          if(client.subscribe(root_topic_subscribe) && client.subscribe(root_topic_tem) && client.subscribe(root_topic_hum))     //aqui es donde 
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
      mens=incoming;                                           //guardo en una variable para poder usarlo  como lectura
    incoming.trim();
    Serial.println("Mensaje: " + incoming);
      casos();                                                 // llama la funcion y dependiendo la orden ejecuta
  
  }
