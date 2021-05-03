
#include <DHT.h>    // importa la Librerias DHT
#include <DHT_U.h>

int SENSOR = 7;     // pin DATA de DHT22 a pin digital 2
float TEMPERATURA;
int HUMEDAD;
int LED = 13;   

DHT dht(SENSOR, DHT11);   // creacion del objeto, cambiar segundo parametro
        // por DHT11 si se utiliza en lugar del DHT22
void setup(){
  Serial.begin(9600);   // inicializacion de monitor serial
  dht.begin();      // inicializacion de sensor
  pinMode(LED, OUTPUT);  
}

void loop(){
    leertemp();
  delay(500);
}

  void leertemp()
  {
    TEMPERATURA = dht.readTemperature();  // obtencion de valor de temperatura
    HUMEDAD = dht.readHumidity();   // obtencion de valor de humedad
    Serial.print("Temperatura: "+String (TEMPERATURA)+" Humedad: "+String(HUMEDAD));  // escritura en monitor serial de los valores
      if (TEMPERATURA>=34) digitalWrite(LED, HIGH);
      else { digitalWrite(LED, LOW);} 
  }
