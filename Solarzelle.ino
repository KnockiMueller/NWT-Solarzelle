#include <Servo.h>


/* Integer
lightlevel1 -> Wert von LDR1
lightlevel2 -> Wert von LDR2
faktor -> Vor oder Zurueck
position -> Aktuelle Position des Servos
minlight -> Licht ab dem man von Fehlstellung ausgehen kann
ulight -> Wert zur Ueberpruefung der Anzahl der Resatarts
*/

int lightlevel1, lightlevel2, faktor, position, minlight, ulight;

/* floats
adif -> Durchschnittliche Differenz
*/
float adif;

/* Konsanten
LDR1 -> Pin LDR1
LDR2 -> Pin LDR2
*/
#define LDR1 A0
#define LDR2 A1

Servo S1;


int functions(int t, int it = 10, int time = 1000) { // Integer Return Funktionen
  // Die Durchschnittlichen Messungenauigkeiten berechnen
  if (t == 1) {
    int sumdif = 0;
    for (int i=0; i <= it; i++) {
      sumdif += analogRead(A0) - analogRead(A1);
      delay(time/it);
    }
    return sumdif/it;
  }

  // Die Startposition ermitteln
  if (t == 2) {
    int lightlevel = 1025;
    int pos = 0;
    S1.write(0);
    for (int i=0; i <= 180; i++) {
      S1.write(i);
      if (analogRead(LDR1) < lightlevel) {
        pos = i;
        lightlevel = analogRead(A0);
      }
      if (analogRead(LDR1) > minlight) {
        minlight = analogRead(LDR1);
      }
      delay(time/180);
    }
    Serial.println(pos);
    S1.write(pos);
    return pos;
  }

}

void setup() {
  /* Initialisierung von allem
  1. Motor einrichten
  2. Serielle Uebertragung starten
  3. Durchschnittliche Messungenauigkeit berechnen
  4. Startposition bestimmen
  */

  S1.attach(3);
  S1.write(0);

  Serial.begin(9600);
  Serial.println("Hi");

  delay(1000);

  adif = functions(1);

  Serial.println(adif);

  position = functions(2);

  Serial.println(minlight);

}


void loop() {
  faktor = 0; // Zuständig fuer Vor oder Zurueck
  lightlevel1 = analogRead(LDR1); // Messung LDR1
  lightlevel2 = analogRead(LDR2); // Messung LDR2
  
  /*
  Serial.print(lightlevel1);
  Serial.print(" ");
  Serial.println(lightlevel2);
  */

  if (tolerance(lightlevel1, lightlevel2, adif + abs(adif * 5))) { // Checkt ob in Toleranzbereich, wenn nicht, Ermittlung ob Vor oder Zurueck
    if (lightlevel1 < lightlevel2) {
      faktor = 1;
    }
    if (lightlevel1 > lightlevel2) {
      faktor = -1;
    }
  }

  if (position + faktor >= 0 && position + faktor <= 180 && faktor != 0) { // Checkt ob ueber 180 oder unter 0 Grad
    position += faktor;
    S1.write(position);
    Serial.println(position);
  }


  if (lightlevel1 > minlight && lightlevel2 > minlight) { // Schaut ob falsche Position sein koennte und evtl. Neustart
    if (ulight%10 == 0 && ulight <= 30 ) {
      Serial.println("Restart");
      adif = functions(1);
      position = functions(2);
    } else {
      ulight += 1;
    }
  } else {
    ulight = 0;
  }

  delay(50);
}


bool tolerance(int x, int y, int t_bereich) {
  
  /*
  x -> Wert 1
  y -> Wert 2
  t_bereich -> Toleranzbereich
  */

  if (x-y < t_bereich && x-y > t_bereich * -1) {
    return false;
  } else {
    return true;
  }
}



