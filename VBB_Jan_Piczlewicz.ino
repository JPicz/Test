/* Jan Lukas Piczlewicz, Matrikelnummer: 201328887, 17.12.2020

Veränderbarer blinkender Balancierer V1.0
 
Programm zur Verwendung des "Schedulers", das mithilfe der RGB-LED
und des Beschleunigungssensors in Abhängigkeit der Neigung die Blinkfrequenz der RGB-LED verändert.
Dabei wird mithilfe von globalen Variablen eine Kommunikation zwischen den einzelnen Schleifen sichergestellt.
Zusätzlich kann der Benutzer über die Serielle Schnittstelle durch Eingabe einer Zahl die LED auf (0 = aus, 1 = rot, 2 = gelb, 3 = grün) umschalten.
*/



#include <Arduino_LSM9DS1.h>                // Einbinden der Bibliothek zur Verwendung des Beschleunigungssensors.
#include <Scheduler.h>                      // Einbinden der Bibliothek zur Werwendung der Scheduler-Methode.
#define POSITIVEABWEICHUNGZULAESSIG 0.3     // Erlaubte Abweichung in [m/s^2] für positive "ax"
#define NEGATIVEABWEICHUNGZULASESSIG -0.3   // Erlaubte Abweichung in [m/s^2] für negative "ax"
#define ERDBESCHLEUNIGUNG 9.81              // Erdbeschleunigung in [m/s^2]
int verzoegerung;                               // globale Variable für die Blinkverzögerung der RGB-LED in [ms]
int farbe;                                  // globale Variable für die Zuweisung bzw. Änderung der Farbe der RGB-LED  


void setup()
{
  Serial.begin(9600);                       // Schnittstelle wird initialisiert und auf eine übertragung von 9600 Baud eingestellt.
  verzoegerung = 1000;                       // Startverzögerung von 1000 ms
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
  float ax, ay, az;                                  // Beschleunigungskomponenten in den jeweiligen Koordinatenrichtungen.          
  ax = 0;                                            // Wir nehmen an, dass es keine Beschleunigung bei Beginn des Programms gibt.
  ay = 0;
  az = 0;
  if(IMU.accelerationAvailable())                    // If-Anweisung, die zur Kontrolle des Beschleunigungssensors dient.
  { 
    IMU.readAcceleration(ax, ay, az);                           // Liest die Beschleunigungswerte des Sensors aus und speichert sie in die genannten Variablen ab.
    if((ax*ERDBESCHLEUNIGUNG) < (NEGATIVEABWEICHUNGZULASESSIG)) // Überprüft Neigung nach vorn. Die Beschleunigung "ax" ist hier negativ.
    {  
      verzoegerung = 100;                                       // Wert der Verzögerung wird auf 100 ms reduziert. 
    }
    if((ax*ERDBESCHLEUNIGUNG) > (POSITIVEABWEICHUNGZULAESSIG))  // Analog zu vorheriger If-Anweisung. Hier ist die Beschleunigung von "ax" jedoch positiv.
    {
      verzoegerung = 3000;                                      // Wert der Verzögerung wird auf 3 s erhöht.
    }
    if((ax*ERDBESCHLEUNIGUNG) > (NEGATIVEABWEICHUNGZULASESSIG) && (ax*ERDBESCHLEUNIGUNG) < (POSITIVEABWEICHUNGZULAESSIG)) // Kontrolliert ob die vorherigen If-Anweisungen beide ausgeschlossen sind.
    {
      verzoegerung = 1000;                                      // Wert der Verzögerung wird auf 1s gesetzt.
    }
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
