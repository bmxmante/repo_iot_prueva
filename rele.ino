/* ArduinO
.... Control digital on/off del pin 13 al pin 11
Conexion del Modulo Bluetooth al Arduino 
usando Pin 0 y 1
*/
#define Pecho 6
#define Ptrig 7
#define Micro 8
int estado=0;

void setup(){
  pinMode(13,OUTPUT);
  pinMode(12,OUTPUT);
  pinMode(11,OUTPUT);
  pinMode(10,OUTPUT);
  pinMode(3,OUTPUT);
pinMode(4,OUTPUT);
pinMode(5,OUTPUT);
  pinMode(Pecho, INPUT);     // define el pin 6 como entrada (echo)
  pinMode(Ptrig, OUTPUT);    // define el pin 7 como salida  (triger)
  pinMode(10, 1);            // Define el pin 13 como salida
  Serial.begin(9600);
  delay(1000);
 }
 // lectura de bluetooth
 void loop(){
 if(Serial.available()>0){           // Si el puerto serie esta habilitadp 
        estado = Serial.read();      // Lee lo que llega por el puerto Serie
 }
 
 // Salidas digitales, dependiendo de lo leido por el puerto serial    
 if(estado== 'a'){                    // on/off de los pin 13 al pin 11
       digitalWrite(13,HIGH);
 }
 if(estado== 'b' ){
       digitalWrite(13,LOW);
 }
 if(estado== 'c'){ 
       digitalWrite(12,HIGH);
 }
 if(estado== 'd' ){
       digitalWrite(12,LOW);
 }
 if(estado== 'e'){ 
       digitalWrite(11,HIGH);
 }
 if(estado== 'f' ){
       digitalWrite(11,LOW);
 }

// Uso de sensor ultrasonido, calculo ditancia
long duracion, distancia;
 
  digitalWrite(Ptrig, LOW);
  delayMicroseconds(2);
  digitalWrite(Ptrig, HIGH);   // genera el pulso de triger por 10ms
  delayMicroseconds(10);
  digitalWrite(Ptrig, LOW);
  
  duracion = pulseIn(Pecho, HIGH);
  distancia = (duracion/2) / 29;            // calcula la distancia en centimetros
  
  if (distancia >= 500 || distancia <= 0){  // si la distancia es mayor a 500cm o menor a 0cm 
    Serial.println("---");                  // no mide nada
  }
  else {
    Serial.print(distancia);           // envia el valor de la distancia por el puerto serial
    digitalWrite(10, 0);               // en bajo el pin 10
  }
  
   if (distancia <= 15 && distancia >= 1){
    digitalWrite(10, 1);     // en alto el pin 10 si la distancia es menor a 15cm
    delay(5000);
  }

// Lectura sensor de humedad
int lectura = analogRead(A0);
Serial.print("La lectura es: ");
Serial.println(lectura);

if (lectura <= 1023 && lectura  >= 900){
  digitalWrite(3,HIGH);
  digitalWrite(4,LOW);
  digitalWrite(5,LOW);
  Serial.println(">> El tanque esta al bajo <<");
}
else if (lectura < 900 && lectura  >= 500){
  digitalWrite(3,HIGH);
  digitalWrite(4,HIGH);
  digitalWrite(5,LOW);
  Serial.println(">> El tanque esta al medio <<");
}
else if (lectura < 500){
  digitalWrite(3,HIGH);
  digitalWrite(4,HIGH);
  digitalWrite(5,HIGH);
  Serial.println(">> El tanque esta al alto <<");
}
delay(500);
 }
