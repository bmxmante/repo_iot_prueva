#include "DHT.h"
#define DHTPIN 15
#define DHTTYPE DHT11


DHT dht(DHTPIN, DHTTYPE);

    // defines pins numbers
    const int trigPin = 2;
    const int echoPin = 5;

// defines variables
float duration;
double D_ant,Distance;
String stringdistance;
String JSON;
float  h_ant,t_ant;

void setup()
{
    pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
    pinMode(echoPin, INPUT);  // Sets the echoPin as an Input
    Serial.begin(9600);       // Starts the serial communication
    dht.begin();
}

void loop()
{ 
    float h = dht.readHumidity();
    float t = dht.readTemperature();
  
    // limpia los pines de trigpin 
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);

    // coloca el trigpin en alto durante 10 microsegundos
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // lee el pin echo y debuelvo el tiempo de ida y vuelta de la onda en microsegundos
    duration = pulseIn(echoPin, HIGH);

    // Calculating the distance
    Distance = duration * 0.034 / 2;
    
    //creacion del jeison
 //   JSON ="{\n \"ID\": 001, \n\"id_casa\":002 \n \"variavle\": [\n{\"temperatura\": 35 C},\n{\"Humedad\": 85 %},\n{\"distancia Cm\": 10 Cm},\n{\"distancia mm\": 100 mm}\n ]\n}"

    // Prints the distance on the Serial Monitor
    if(D_ant != Distance || h_ant != h || t_ant != t)
      {
        Serial.print("Distancia: ");
        Serial.println( String (Distance) + " Cm");
        Serial.print(F("Humedad: "));
        Serial.print(h);
        Serial.print(F("% Temperatura: "));
        Serial.print(t);
        Serial.println(F("°C "));
      }
      D_ant=Distance;
      h_ant=h;
      t_ant=t;
      delay(1000);
   // Serial.print(JSON);
}
