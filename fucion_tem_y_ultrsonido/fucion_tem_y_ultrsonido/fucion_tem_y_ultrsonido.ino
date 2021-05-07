//#include <DHT.h>
//#include DHTPIN 4

//DHT dht(DHTPIN, DHTTIPE)

    // defines pins numbers
    const int trigPin = 2;
    const int echoPin = 5;

// defines variables
long duration;
int Distance;
String stringdistance;
String JSON;
void setup()
{
    pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
    pinMode(echoPin, INPUT);  // Sets the echoPin as an Input
    Serial.begin(9600);       // Starts the serial communication
//    dht.begin();
}

void loop()
{
    // Clears the trigPin
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);

    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);

    // Calculating the distance
    Distance = duration * 0.034 / 2;
    
    //creacion del jeison
    JSON ="{\n \"ID\": 001, \n\"id_casa\":002 \n \"variavle\": [\n{\"temperatura\": 35 C},\n{\"Humedad\": 85 %},\n{\"distancia Cm\": 10 Cm},\n{\"distancia mm\": 100 mm}\n ]\n}"

    // Prints the distance on the Serial Monitor
    Serial.print("Distance: ");
    Serial.println(stringdistance + " Cm");
    Serial.print(JSON);
}
