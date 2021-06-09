/* Jan Lukas Piczlewicz, Matrikelnummer: 201328887, 18.12.2020

Veränderbarer blinkender Balancierer V1.1
 
Programm zur Verwendung des "Schedulers", das mithilfe der RGB-LED
und des Beschleunigungssensors in Abhängigkeit der Neigung die Blinkfrequenz der RGB-LED verändert.
Dabei wird mithilfe von globalen Variablen eine Kommunikation zwischen den einzelnen Schleifen sichergestellt.
Zusätzlich kann der Benutzer über die Serielle Schnittstelle durch Eingabe einer Zahl die LED auf (0 = aus, 1 = rot, 2 = gelb, 3 = grün) umschalten.
*/



#include <Arduino_LSM9DS1.h>                // Einbinden der Bibliothek zur Verwendung des Beschleunigungssensors.
#include <Scheduler.h>                      // Einbinden der Bibliothek zur Werwendung der Scheduler-Methode.
                                            // Konstanten wurden entfernt, da sie hier nicht mehr verwendet werden.
int verzoegerung;                           // globale Variable für die Blinkverzögerung der RGB-LED in [ms]
int farbe;                                  // globale Variable für die Zuweisung bzw. Änderung der Farbe der RGB-LED  


void setup()
{
  Serial.begin(9600);                       // Schnittstelle wird initialisiert und auf eine übertragung von 9600 Baud eingestellt.
  verzoegerung = 1000;                      // Startverzögerung von 1000 ms
  farbe    = 1;                             // Setze die Farbe der LED auf rot
  pinMode(LEDR, OUTPUT);                    // weise die Pins der RGB-LED als Ausgänge zu.
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);

  if(!IMU.begin())                            // If-Anweisung, die zur Kontrolle des Beschleunigungssensors dient.
  {
    Serial.println("Initialisierung des IMU fehlgeschlagen!");
    while(1);
  }
  
  Scheduler.startLoop(loopUeberPruefeAusrichtung);  // Aufruf der startLoop Methode startet die Schleifen der Überprüfung der Ausrichtung
  Scheduler.startLoop(loopRgbLedFarbeAendern);      // und der Farbeänderung der RGB-LED.
}
void loop ()                                        // Standard Schleife, die die LED blinken lässt.
{
  statusLedAn();                                    // LED wird angeschaltet.
  delay(verzoegerung);                                  // Verzögerung greift auf den Wert der Frequenzvariable zu.
  statusLedAus();                                   // LED wird ausgeschaltet.
  delay(verzoegerung);                                  
}

void loopUeberPruefeAusrichtung()                    // Schleife, die die Ausrichtung des Nano BLE 33 in x-y Ebene überprüft.
{
  float ax, ay, az;                                  // Beschleunigungskomponenten in den jeweiligen Koordinatenrichtungen in [g]. 1g entspricht hierbei ca. einer Beschleunigung von 9,81 m/s.         
  ax = 0;                                            // Wir nehmen an, dass es keine Beschleunigung bei Beginn des Programms gibt.
  ay = 0;
  az = 0;
  if(IMU.accelerationAvailable())                    // If-Anweisung, die zur Kontrolle des Beschleunigungssensors dient.
  { 
    IMU.readAcceleration(ax, ay, az);                // Liest die Beschleunigungswerte des Sensors aus und speichert sie in die genannten Variablen ab.
    if(ax > -1)                                      // Obligatorische Abfrage, da Werte kleiner als -1g zu einem negativen Wert für die Verzögerung führen. Dies führte beim Test zu einem Absturz des Programms.
    {                                                // Die Abfrage ist also notwendig, da bei einer vertikalen Neigung von 90 Grad nach unten, der Wert für ax kleiner als -1g ist (Merkwürdig).
      verzoegerung = ax*1000+1000;                   // Funktion für die Verzögerung [ms] in Abhängigkeit zur Beschleunigung der Komponente ax. Ist der Arduino horizontal ausgerichtet, leuchtet die LED mit einer Standardverzögerung von 1000ms 
    }                                                // Die vorherige Überprüfung der Neigung wird nicht mehr benötigt. (Konstanten wurden entfernt)
  }
}

void loopRgbLedFarbeAendern()                       // Schleife, die die Farbe der LED ändert.
{
  if (Serial.available())
  {
    char eingabe = Serial.read();                   // Speichert das nächste eingegebene Zeichen des Benutzers in die Variable "eingabe" ab.                       
    if (eingabe == '0')                             // Wenn das Zeichen der Zahl "0" entspricht,
    {
      farbe = 0;                                    //wird die Variable "farbe" auch auf "0" gesetzt. Die Variable dient hierbei zum Aufrufen der jeweiligen Methode zum Aus- oder Umschalten der LED.
      Serial.println("LED ausgeschaltet.");         // Rückmeldung für den Benutzer, dass die Farbe geändert bzw die LED ausgeschaltet wurde.
    }
    if (eingabe == '1')                             // If-Anweisung für die Farbe rot.
    {
      farbe = 1;
      Serial.println("LED auf rot umgeschaltet.");  
    }
    if (eingabe == '2')                             // If-Anweisung für die Farbe gelb.
    {
      farbe = 2;
      Serial.println("LED auf gelb umgeschaltet.");    
    }
    if (eingabe == '3')                             // If-Anweisung für die Farbe grün.
    {
      farbe = 3;
      Serial.println("LED auf gruen umgeschaltet.");    
    }
  }
  yield();                                          // Da in dieser Schleife niemals eine zeitliche Verzögerung auftritt, wird mit "yield()" dafür gesorgt, dass auch andere Schleifen die Chancen bekommen abgearbeitet zu werden.
}

void statusLedAn()                                 // Methode, die für das Anschalten der LED in der jeweiligen Farbe zuständig ist.
{
  if (farbe == 0)
  {
    statusLedAus();                                  // Aufruf der Methode zum Ausschalten der LED.
  }
  if (farbe == 1)                                  // If-Anweisung für die Farbe rot.
  {
    digitalWrite(LEDR, LOW);                       // rote "Komponente" an
    digitalWrite(LEDG, HIGH);                      // grüne "Komponente" aus
    digitalWrite(LEDB, HIGH);                      // blaue "Komponente" aus
  }
  if (farbe == 2)                                  // If-Anweisung für die Farbe gelb.
  {
    digitalWrite(LEDR, LOW);                      
    digitalWrite(LEDG, LOW);
    digitalWrite(LEDB, HIGH);
  }
  if (farbe == 3)                                  // If-Anweisung für die Farbe grün.
  {
    digitalWrite(LEDR, HIGH);
    digitalWrite(LEDG, LOW);
    digitalWrite(LEDB, HIGH);
  }
}

void statusLedAus()                               // Methode, die alle Komponenten der RGB-LED ausschaltet.
{
  digitalWrite(LEDR, HIGH);  // rote "Komponente" aus
  digitalWrite(LEDG, HIGH);  // grüne "Komponente" aus
  digitalWrite(LEDB, HIGH);  // blaue "Komponente" aus 
}
