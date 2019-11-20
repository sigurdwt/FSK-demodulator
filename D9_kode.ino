#include <TimerOne.h>
/* Om du ikke har innstallert dette biblioteket
gå til Sketch -> Include library -> Manage Libraries
Søk opp TimerOne og innstaller*/

// Globale variaber
volatile int sample; // Holder siste sample
bool newSample; // Støtte varibel for å sjekke om ny sample er tatt
float a=0.5;    //Variabel til filterene
int offset=205;  //Kompensering for tillagt offset på inngangssignalet, 
                 //da ADC-en ikke leser negative spenningsverdier.

float hPas=0;   //r_(HPF)[n]
float lPas=0;   //r_(LPF)[n]

int count=0;    //Støttevariabel 
int maxCount=20;  //Antall punktprøver, N, for estimat av amplitude
float sumH=0; 
float sumL=0;
float midH=0;
float midL=0;

void setup() {
  pinMode(8, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(0, INPUT);

  // Oppsett av timer interrupt
  Timer1.initialize(500); // 500 mikrosekund mellom hver sample -> gir F_s = 2kHz
  // Argumentet i "attachInterrupt" bestemmer hvilken funskjon som er interrupt handler 
  Timer1.attachInterrupt(takeSample); 
}

void loop() {
  if(newSample){    // Ny sample er tatt
    sample=sample-offset;
    hPas=sample-a*hPas;   //rekursivt høypassfilter
    lPas=sample+a*lPas;   //rekursivt lavpassfilter

    sumH+=abs(hPas);
    sumL+=abs(lPas);

    if(++count==maxCount){
      midH = sumH/maxCount;
      midL = sumL/maxCount;
      
      if(midH<=20 && midL<=20){
        digitalWrite(7,LOW);
        digitalWrite(8,LOW);
      }
      else{
        digitalWrite(7,HIGH);
        if(midH>midL){
          digitalWrite(8, HIGH); 
        }
        else{
          digitalWrite(8, LOW);
        }
      }
      sumH=0;
      sumL=0;       //Nullstiller verdiene
      count=0;
    }
    newSample = false;
  }
}

// Interrupt-handler (denne kalles ved hvert interrupt)
void takeSample(void){
  sample = analogRead(0); // Sampler på A0
  newSample = true;
}
