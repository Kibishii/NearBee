#include <Adafruit_NeoPixel.h>
// Zuweisung der Board-Ausgaenge:

#define trigPinB 0
#define echoPinB 1

#define trigPinR 2
#define echoPinR 3

#define trigPinL 6
#define echoPinL 12

#define vibratorL 9
#define vibratorR 10

#define led 8

// Deklaration --- Start
Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, led, NEO_GRB + NEO_KHZ800);
int waitTime = 1;
bool isVibL=false;
bool isVibR=false;
bool isVibB=false;
int alarm =0;
int savePosition =0;
int cirtDistance = 150; // Default 150
int warnDistance = 200; // Default 200
int maxOkDistance = warnDistance +1;
int distanceSaveR[] = {maxOkDistance, maxOkDistance, maxOkDistance};
int distanceSaveL[] = {maxOkDistance, maxOkDistance, maxOkDistance};
int distanceSaveB[] = {maxOkDistance, maxOkDistance, maxOkDistance};
// Deklaration --- Ende

void setup() {
  // Zuweisung der In- und Outputs:
  pinMode(trigPinR, OUTPUT);
  pinMode(echoPinR, INPUT);
  pinMode(trigPinB, OUTPUT);
  pinMode(echoPinB, INPUT);
  pinMode(trigPinL, OUTPUT);
  pinMode(echoPinL, INPUT);
  pinMode(vibratorR, OUTPUT);
  pinMode(vibratorL, OUTPUT);
  // Vorbereitung für das Onboard-Neopixel
  strip.begin();
  strip.setBrightness(50);
  strip.show();
  // Serielle Ausgabe ueber 9600:
  Serial.begin(9600);
}

void loop() {
  // alarm = 1 bis 3 = Person auf mittlerer Entfernung; alaram = 4 und höher Person zu nahe
  alarm =0;
  // Berechung der Entfernungen
  distanceSaveR[savePosition] = checkDistance(trigPinR, echoPinR);
  distanceSaveL[savePosition] = checkDistance(trigPinL, echoPinL);
  distanceSaveB[savePosition] = checkDistance(trigPinB, echoPinB);
  
  // Pruefen welche Vibratoren verwendet werden sollen
  isVibR = checkSide(distanceSaveR, isVibR, "Right");
  isVibL = checkSide(distanceSaveL, isVibL, "Left");
  isVibB = checkSide(distanceSaveB, isVibB, "Back");

  // Verwendung des Onboard-Neopixels
  if(alarm >= 4){
    strip.setPixelColor(0,strip.Color(255, 0, 0));
  } else if(alarm >=1){
    strip.setPixelColor(0,strip.Color(0, 0, 255));
  }else{
    strip.setPixelColor(0,strip.Color(0, 255, 0));
  }
  strip.show();

  // Starten der Vibratoren
  if(isVibB || isVibR){
    digitalWrite(vibratorR, HIGH);
  }else{
    digitalWrite(vibratorR, LOW);
  }
  if(isVibB || isVibL){
    digitalWrite(vibratorL, HIGH);
  }else{
    digitalWrite(vibratorL, LOW);
  }
  // Vorbereitung für den naechsten loop
  savePosition +=1;
  if(savePosition >= 3){
    savePosition =0;
  }
  delay(waitTime);
}

// Ermittlung von Distanzen
int checkDistance(int trigger, int echo){
  digitalWrite(trigger, LOW);
  delayMicroseconds(5);
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);
  long duration = pulseIn(echo, HIGH);
  // Berechnung der Distanz:
  int result =duration * 0.034 / 2;
  return result;
}

// Pruefung ob eine Distanz unterschritten wurde
bool checkSide(int distanceArray[], bool vib, String from){
  bool actVib = vib;
  int distance = maxOkDistance;
  for(int i; i <= sizeof(distanceArray); i++){
    if(distance > distanceArray[i] && distanceArray[i] > 0){
      distance = distanceArray[i];
    }
  }
  Serial.print("DistanceSide = ");
  Serial.print(from);
  Serial.print("==");
  Serial.print(distance);
  Serial.println(" cm");
  if(distance <= cirtDistance){
    actVib =true;
    alarm +=4;
  } else if(distance <=warnDistance){
    actVib =!actVib;
    alarm +=1;
  }else{
    actVib =false;
  }
  return actVib;
}
