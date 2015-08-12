#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define turn_page 6
#define reed 5 //pin a cui collego il sensore reed
LiquidCrystal_I2C lcd (0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); //scl=A5, sda=A4
float c = 2.07470779; //lunghezza ruota (da 26'') in metri
float V = 0; //Velocità
float A = 0; //Accelerazione
float S = 0; //Distanza percorsa a partire dall'ultima corsa
float Km = 0; //distanza totale percorsa in Km
unsigned long t1, t2 = 0; //Variabili temporali 1 e 2
unsigned long int guard = 0; //Variabile che controlla se sono fermo
//Clock??
float timer = 0; //Cronometro
int counter = 0; //Var che conta gli impulsi del sensore reed (numero di giri della ruota)
int page = 1; ///Var che mi dice a che pagina è il display

void Round(int sheet) {  //funzione che alla pressione del tasto turn_page cambia la pagina visualizzata sul display lcd
  switch (sheet) {
    case 1: { //Distanza parziale e velocità
        lcd.clear();
        lcd.print("Par="); lcd.print(S); lcd.print(" Km");
        lcd.setCursor(0, 1);
        lcd.print("Vel="); lcd.print(V); lcd.print(" Km/h");
        break;
      }
    case 2: { //Distanza totale e accelerazione
        lcd.clear();
        lcd.print( "Tot="); lcd.print(Km); lcd.print(" Km");
        lcd.setCursor(0, 1);
        lcd.print("Acc="); lcd.print(A); lcd.print(" m/s^2");
        break;
      }
  }
  delay(100);
}

void Refresh(int sheet){ //funzione che aggiorna i valori visualizzati adesso senza riscrivere l'intera pagina
  switch (sheet) {
    case 1:{
      lcd.setCursor(4,0);
      lcd.print(S);
      lcd.setCursor(4,1);
      lcd.print(V);
      }
    break;
    case 2: {
      lcd.setCursor(4,0);
      lcd.print(Km);
      lcd.setCursor(4,1);
      lcd.print(A);
      }
      break;
   }
}



void setup() {
  int page = 1;
  lcd.begin(16, 2);
  pinMode(reed, INPUT);
  pinMode(turn_page, INPUT);
  lcd.print("Ciao Elia,");
  lcd.setCursor(0, 1);
  lcd.print("Buona corsa! :)");
  delay(3000);
  Round(page);
}

void loop() {
  guard = millis();
  if ((guard - t2) > 2000) { //controllo se sono fermo
    V = 0;
    Refresh(page);
      }
  if (digitalRead(reed) == HIGH) { //controllo se il reed ha dato un'impulso e calcolo distanza e velocità
    t1 = t2;
    t2 = millis();
    V = (c *  3600  / (t2 - t1));
    Km = Km + (c / 1000);
    Refresh(page);
    delay(200);
   }

  if (digitalRead(turn_page) == HIGH) {
    //page = (page + 1) % 2; IN INFORMATICA SI CONTA DA ZERO CAPRA
    if (page == 1) {
      page = 2;
    }
    else if (page == 2) {
      page = 1;
    }
    Round(page);
    delay(100);
}


//andando a 50km/h faccio un giro di ruota ogni 11ms è inutile ripetere più spesso il loop e non rischio di prendere due volte lo stesso passaggio della ruota

//  lcd.clear();
//  lcd.print(c);
//  t1 = millis();
//  delay(1000);
//  t2 = millis();
//  lcd.clear();
//  lcd.print(t2-t1);
//  lcd.setCursor(0,1);
//  lcd.print(c*3600/(t2-t1));
//  delay(1000);
}

