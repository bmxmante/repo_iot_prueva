#include <ArduinoLowPower.h>
int RECV_PIN = 11; //declaramos el pin que recibira la señal 
IRrecv irrecv(RECV_PIN); 
decode_results results; 

void setup() { 
Serial.begin(9600); //inicializamos el puerto serial a 9600 
irrecv.enableIRIn(); // Comienza a recibir los datos 
} 

void loop() { 
if (irrecv.decode(&results)) { 
Serial.println(results.value, HEX); // imprime el valor en codigo hexadecimal 
irrecv.resume(); // Se preparar para recibir el siguiente valor 
delay(10); 
} 
}
