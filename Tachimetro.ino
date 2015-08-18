#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Math.h>

#define button 6 //pin a cui collego il bottone per agire sul menù
#define reed 5 //pin a cui collego il sensore reed
#define menu 8 //pin per interagire con il menù (start/stop timer, azzeramento parziale ecc)

LiquidCrystal_I2C lcd (0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); //scl=A5, sda=A4
float c = 2.07470779; //lunghezza ruota (da 26'') in metri
float V = 0; //Velocità
int an_speed = 0; //Variabile per la visualizzazione analogica della velocità
float A = 0; //Accelerazione
float P = 0; //Distanza percorsa a partire dall'ultima corsa
float Km = 0; //distanza totale percorsa in Km
unsigned long t1, t2 = 0; //Variabili temporali 1 e 2
unsigned long int guard = 0; //Variabile che controlla se sono fermo
unsigned long time_start, timer = 0; //Istante di inizio del cronometro e tempo trascorso dall'istante d'inizio
int h, m, s = 0; //Ore, minuti e secondi per il cronometro
boolean start_stop = false; //Booleano per verificare se il cronometro è avviato o fermo. falso finchè il cronometro è fermo
int page = 0; ///Var che mi dice a che pagina è il display
boolean double_click = false; //Booleano che indica la pressione doppia del pulsante per agire sul menù

/* Creazione dei caratteri per indicare la velocità in maniera analogica e visiva */

/*byte five[8] = { B11111, B11111, B11111, B11111, B11111, B11111, B11111, B11111 };

byte four[8] = { B11110, B11110, B11110, B11110, B11110, B11110, B11110, B11110 };

byte three[8] = { B11100, B11100, B11100, B11100, B11100, B11100, B11100, B11100 };

byte two[8] = { B11000, B11000, B11000, B11000, B11000, B11000, B11000, B11000};

byte one[8] = { B10000, B10000, B10000, B10000, B10000, B10000, B10000, B10000 };*/
const uint8_t matrix[][8] ={
 {
  0x10,
  0x10,
  0x10,
  0x10,
  0x10,
  0x10,
  0x10,
  0x10},

{
  0x18,
  0x18,
  0x18,
  0x18,
  0x18,
  0x18,
  0x18,
  0x18},

 {
  0x1C,
  0x1C,
  0x1C,
  0x1C,
  0x1C,
  0x1C,
  0x1C,
  0x1C},

{
  0x1E,
  0x1E,
  0x1E,
  0x1E,
  0x1E,
  0x1E,
  0x1E,
  0x1E},
  
  {
  0x1f,
  0x1f,
  0x1f,
  0x1f,
  0x1f,
  0x1f,
  0x1f,
  0x1f}
  };

/* Gestione della visualizzazione analogica della velocità. Ogni colonna illuminata equivale a 2 Km/h, si legge fino ad un massimo di 40 Km/h (uso 4 caratteri) */

void analog_speed () {
  lcd.setCursor(10,1);
  if (V < 40) {
    an_speed = round(V / 2);
  }
  else {
    an_speed = 20;
  }
  for (int i = 0; i < an_speed/5; i++) {
        lcd.write(5);
      }
      lcd.write( ( an_speed % 5 ) );
    }


/* Funzioni per la gestione del refresh e cambio del menu sull'lcd */

void Round(int page) {  //funzione che alla pressione del tasto sul pin button cambia la pagina visualizzata sul display lcd
  switch (page) {
    case 0: { //Distanza parziale e velocità
        lcd.clear();
        lcd.print("Par="); lcd.print(P); lcd.print(" Km");
        lcd.setCursor(0, 1);
        lcd.print("Vel="); lcd.print(V); lcd.print(" Km/h");
        break;
      }
    case 1: { //Distanza totale e cronometro
        lcd.clear();
        lcd.print( "Tot="); lcd.print(Km); lcd.print(" Km");
        lcd.setCursor(0, 1);
        if (start_stop == true ) { //se il cronometro non è stato avviato, timer non si deve aggiornare ma rimanere a 0
          timer = (millis() - time_start) / 1000;
        }
        Refresh_Chronometer();
        //analog_speed();
        break;
      }
  }
}

void Refresh(int page) { //funzione che aggiorna i valori visualizzati sulla pagina attuale senza riscrivere l'intera pagina
  if (start_stop == true ) { //se il cronometro non è stato avviato, start_stop deve rimanere a 0, altrimenti si aggiorna anche se sono in pagina 0, poi non verrà visualizzato
    timer = (millis() - time_start) / 1000;
  }
  else {
    m, h, s = 0;
  }
  switch (page) {

    case 0: {
        lcd.setCursor(4, 0);
        lcd.print(P);
        lcd.setCursor(4, 1);
        lcd.print(V);

        break;

      }
    case 1: {
        lcd.setCursor(4, 0);
        lcd.print(Km);
        Refresh_Chronometer();
        //analog_speed();
        break;
      }
  }
}


boolean chronometer(boolean start_stop) { //Gestione del cronometro
  if (start_stop == false) { // se è fermo lo devo avviare il cronometro
    time_start = millis(); //Timer contiene l'istante in cui è stato avviato il cronometro
    return true; //Ne segnalo l'avvio
  }
  else {
    h, m, s = 0;
    return false;
  }
}

void Refresh_Chronometer() {
lcd.setCursor(1,1);
  if (h < 10) {
    lcd.print("0");
    lcd.print(h);
    lcd.print(":");
  }
  else {
    lcd.print(h);
    lcd.print(":");
  }
  if (m < 10) {
    lcd.print("0");
    lcd.print(m);
    lcd.print(":");
  }
  else {
    lcd.print(m);
    lcd.print(":");
  }
  if (s < 10) {
    lcd.print("0");
    lcd.print(s);
  }
  else {
    lcd.print(s);
  }

}


void setup() {
  pinMode(reed, INPUT);
  pinMode(button, INPUT);
  pinMode (menu, INPUT);
  lcd.begin(16, 2);
  lcd.createChar(0, (uint8_t *)matrix[0]);
 // lcd.createChar(2, two);
 // lcd.createChar(3, three);
 // lcd.createChar(4, four);
  //lcd.createChar(5, five); 
  
  //lcd.print("Ciao Elia,");
  lcd.setCursor(0, 1);
  //lcd.print("Buona corsa! :)");
  int i = 0;
  lcd.print(char(i));

  delay(10000);
  Round(page);
}

void loop() {
  /*        Model View Control
  Aggiornamento modello interno*/

  /*Diplay*/

  /*Controllo*/
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
    P = P + (c / 1000);
    Refresh(page);
    delay(120); //andando a 50km/h faccio un giro di ruota ogni 150ms è inutile ripetere più spesso il loop e non rischio di prendere due volte lo stesso passaggio della ruota
  }

  if (digitalRead(button) == HIGH) { //Per cambiare pagina del display lcd
    page = (page + 1) % 2; //IN INFORMATICA SI CONTA DA ZERO CAPRA
    Round(page);
    delay(120);
  }

  if (digitalRead(menu) == HIGH) { //riconosce la pressione del pulsante di menù e su quale pagina è stato premuto
    delay(2000);
    switch (page) {
      case 0:
        P = 0; //azzera il parziale
        break;
      case 1:
        start_stop = chronometer(start_stop); //avvia la funzione di gestione del cronometro
        break;
    }
  }

  if ( ( (timer - (h * 3600 + m * 60 + s) ) >= 1 && start_stop == true ) ) { //aggiornamento del tempo del cronometro, lo valuto solo se il cronometro è stato avviato e in ogni caso ogni secondo
    h = timer / 3600; //definizione variabili del cronometro
    m = (timer - h * 3600) / 60;
    s = (timer - h * 3600 - m * 60);
  }
}
//per azzerare il cronometro un doppio doppio click in pagina 1?
//barrettine d'acceelerazione o velocità
//se stacco tutto, memorizzo nella eeprom il totale
