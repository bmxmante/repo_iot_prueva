#include <DHT.h>    // importa la Librerias DHT sensor de temperaturo y humedad 
#include <DHT_U.h>
#include <SoftwareSerial.h>
SoftwareSerial modulo(2,3); //defino modulo como serial en los puertos (2,3)  (pin 2) Tx  (pin 3)  Rx

int on_off,estado,estado2;                //defino on_off, estado como un entero 
char caracteres;                              //defino estado como una cadena de carateres
int LED = 13;       //ROJO                          //defino led como un entero en el puerto 13
int LED1 = 12;                                 //defino led como un entero en el puerto 12
String Wed,cw,comandoCerrar, prueva,estado1 ;  //defino W,cw y comandoCerrar como cadenas de caracteres 
int SENSOR = 4;                              // pin del sensor DHT11 como un entero en el puerto 11
float TEMP;                                   //defino temp como un decimal 
int HUMEDAD; 
int VENT = 11,TEM; 

DHT dht(SENSOR, DHT11);                       // envio sensor a la funcion interna y digo que voy a trabajar con un dht11

    void setup()
    {
        Serial.begin(9600);               // monitor serial del arduino
        modulo.begin(9600);               // baud con los que voy a trabajar el ESP8255
        dht.begin();
         pinMode(LED, OUTPUT);          //le digo a led como salida
         pinMode(LED1, OUTPUT);          //le digo a led1 como salida
         pinMode(VENT, OUTPUT);          //le digo a led1 como salida
         digitalWrite(LED,HIGH);         //le envio a led un 0 digital (apagado) 
         digitalWrite(LED1,LOW);         //le envio a led1 un 0 digital (apagado) 
         digitalWrite(VENT,LOW);         //le envio a led1 un 0 digital (apagado) 
        configModuleWifi();               //  llamo la funcion configModuleWifi donde por comandos AT configuro el modulo esp8266
    }
    
    void loop()                                     //bukle infinito
    {   
        TEMP = dht.readTemperature();        // guardo el valor del sensor de temperatura en temp
        HUMEDAD = dht.readHumidity();               // guardo el valor del sensor de humedad en humedad
      
        if(modulo.available())                      //si el puerto serial modulo llega toda la informacion entra
        {      
              if(modulo.find("+IPD,"))              //si el puerto serial modulo encuentra la cadena de carateres +ipd, entra
              {
                delay(1500);                        //espero 1,5seg
                 on_off = modulo.read()-48;          //leo lo que trae el serial modulo en ya que me llega o un 49(1) o un 48(0) entero y le resto 48 para volverlo un 1 o 0 entero
                 modulo.find("PRENDERSISTEMA=");               //busco en el serial modulo la cadena de carateres "led="
                 estado = (modulo.read()-48);        //leo lo que trae el serial modulo en ya que me llega o un 49(1) o un 48(0) entero y le resto 48 para volverlo un 1 o 0 entero 
                 casos(); 
                   
                     digitalWrite(VENT, LOW);
                      while(modulo.available())       //mientras que el puerto serial modulo llega toda la informacion entra
                      {
                        caracteres = modulo.read();     // leo la informacion del modulo y lo guardo en una cadena de caracteres definidad como c
                        Serial.print(caracteres);       //escribo en el puerto serial lo que contenga la cadena de caracteres "caracteres"
                      }
                    //  Serial.print("DATOS RECIBIDOS DE ESTADO");
                      // Serial.print(String (estado));
                  ResponseClient();  //llamo la "funcion responseclient" que es la que muestra en la pag wed si se prendo o se apaga el led
              }
         }
        if(TEMP>=34)prenderventilador(34);              //si el puerto serial modulo encuentra la cadena de carateres +ipd, entra
        else digitalWrite(VENT, LOW); 
    }

      void prenderventilador(int TEM)                                                // funcion "funcion casos"
      {
        while (TEMP>=TEM) 
           {
            TEMP = dht.readTemperature();  
            digitalWrite(VENT, HIGH);
            Serial.print(TEMP); 
            Serial.print("\r\n"); 
            delay(200); 
           }
      }
    

      void casos()                                                // funcion "funcion casos"
      {
          switch (estado) 
          {
            case 0:
              digitalWrite(LED1, LOW);
              digitalWrite(LED, HIGH); 
               digitalWrite(VENT, LOW); 
              break;
              
            case 1:
              digitalWrite(LED1, HIGH);
              digitalWrite(LED, LOW);
              prenderventilador(34);
              break;

             case 2:
              digitalWrite(LED1, HIGH);
              digitalWrite(LED, LOW);
              prenderventilador(32); 
              break;

             case 3:
              digitalWrite(LED1, HIGH);
              digitalWrite(LED, LOW);
              prenderventilador(36); 
              break;

             case 4:
              digitalWrite(LED1, HIGH);
              digitalWrite(LED, LOW);
              prenderventilador(38); 
              break;

             case 5:
              digitalWrite(LED1, HIGH);
              digitalWrite(LED, LOW);
              prenderventilador(40); 
              break;

            case 6:
              digitalWrite(LED1, HIGH);
              digitalWrite(LED, LOW);
              prenderventilador(42); 
              break;

             case 7:
              digitalWrite(LED1, HIGH);
              digitalWrite(LED, LOW);
              prenderventilador(44); 
              break;

             case 8:
              digitalWrite(LED1, HIGH);
              digitalWrite(LED, LOW);
              prenderventilador(46); 
              break;

             case 9:
              digitalWrite(LED1, HIGH);
              digitalWrite(LED, LOW);
              prenderventilador(48); 
              break;  
            default:
              //   Serial.print("NADA"); 
              break;
          }
      }  

      void ResponseClient()                                                // funcion "funcion responseclient"
      {
          Wed = "  ";                                                    //
            if (estado==1) Wed += "<h1>Sistema Encendido!  :)</h1>";     //si el estado es 1 en la consola escribo "led= encendido1:)"
            if (estado==0){ Wed += "<h1> Sistema Apagado!  :(</h1>";}              //si no escribo escribo "led= apagado :("
             Wed += ("<h1>Temperatura: "+String (TEMP)+" Humedad: "+String(HUMEDAD)+"</h1>"); 
             
             
          cw = "AT+CIPSEND=";                                  //guardamos el comando AT en un solo vector de caracteres que indica el mensaje de a escribir en la consola
            cw+=on_off;
            cw+=",";
            cw+=Wed.length();                             //le envio el numero de caracteres en caracter
            cw+="\r\n";
            sendData(cw,1000);                               //llamo la funcion sendata
            sendData(Wed,1000);
          comandoCerrar = "AT+CIPCLOSE=";
            comandoCerrar+=on_off;
            comandoCerrar+="\r\n";
            sendData(comandoCerrar,3000);
      }

      void configModuleWifi()                            // funcion "configmodulewifi"
        {                                                //envial al modulo los comnados AT 
         sendData("AT+RST\r\n",7000);                    // resetear módulo esp8266 y se espera 7 seg
         sendData("AT+CIFSR\r\n",1000);                  // obtener dirección IP y se espera 1 seg
         sendData("AT+CIPMUX=1\r\n",1000);               // configurar para multiples conexiones y se espera 1 seg
         sendData("AT+CIPSERVER=1,80\r\n",1000);         // servidor en el puerto 80 y se espera 1 seg
        }

    void sendData(String comando, const int timeout)     // funcion se
    {
      long int time = millis();
      modulo.print(comando); 
         while( (time+timeout) > millis())
         {
              while(modulo.available())
              { 
                caracteres = modulo.read();               // leer el siguiente caracter
                Serial.print(caracteres);
              }
         } 
         return;
    }
