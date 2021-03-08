//NOMBRE DEL PROYECTO: SMART CARD
//FECHA DE CREACION: 20/11/2020
//NOMBRE DE LA COMPAÑIA: DOMOTICS ELECTRONICS.
//PROPIEDAD INTELECTUAL: DOMOTICS ELECTRONICS.
//DESAROLLADOR: EDMUNDO BARBOSA G.
//JEFE DE DISEÑO ELECTRINICO: EDMUNDO BARBOSA G.
//DURACION DEL PROECTO: 3 MESES
//VESION: 2.1.0.0
/**********LIBERIAS-DOMOTICS**********/
#include <avr/wdt.h>
long intervalWDT = 500;
unsigned long previousMillisWDT = 0;
#include <Wire.h>
#include <NewPing.h>
#include <DHT.h>
#include <Adafruit_GFX.h>   //Libreria para pantallas graficas
#include <Adafruit_SSD1306.h>// libreria para controlador SSD1306
/**********FIN-DE-LBERIAS-DOMOTICS**********/
#define ANCHO 128           //Reemplaza ocurrencia de ANCHO por 128
#define ALTO 64             //Reemplaza ocurrencia de ALTO por 64
#define OLED_RESET 4        //Necesario por la libreria pero no usado
Adafruit_SSD1306 oled(ANCHO, ALTO, &Wire, OLED_RESET);  // crea objeto
/***************ESTABLECEMOS-NUESTROS-PINES***************/
#include "SoftwareSerial.h"
#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"
SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);
/*********PINES-DE-SENSOR-DHT11*********/
#define SEN 2
int temp, humedad;
DHT dht (SEN, DHT11);
/********PINES-Y-VARIABLE-DE-SENSOR-DE-ULTRASONIDO********/
#define TRIGGER_PIN  5  //TRIGGER=D5
#define ECHO_PIN     6  //ECHO=D6
#define MAX_DISTANCE 300//Maximo distancia para medir 400-500cm
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
int distancia = 0;
/*********PINES-DE-R-M-B*********/
char DATO = 0;
#define R1  A0
#define R2  A1
#define R3  A2
#define R4  A3


void setup() {
  wdt_disable();

  Serial.begin(9600);
  mySoftwareSerial.begin(9600);//INICIAR COM. CON ARDUINO
  Wire.begin();//INICIAR BUS I2C
  oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);//INICIAR PANTALLA CON DIRECCIÓN 0X3C
  dht.begin();//INICIAR COM. CON ARDUINO


  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  if (!myDFPlayer.begin(mySoftwareSerial)) {  //SOFTWARESERIAL PARA COMUNICARSE CON EL MP3.
    Serial.println(F("NO SE PUEDE EMPEZAR: "));
    Serial.println(F("1.¡POR FAVOR, VUELVA A COMPROBAR LA CONEXIÓN!"));
    Serial.println(F("2.¡INSERTA LA TARJETA SD!"));
    while (true);
  }


  Serial.println(F("DFPlayer Mini online."));
  myDFPlayer.setTimeOut(500); //Set serial communictaion time out 500ms
  //----AJUSTAR EL VOLUMEN----
  myDFPlayer.volume(30);  //Set volume value (0~30).
  myDFPlayer.volumeUp(); //Volume Up
  myDFPlayer.volumeDown(); //Volume Down
  //----ESTABLECER UN ECUALIZADOR DIFERENTE----
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
  //----ESTABLECER EL DISPOSITIVO SD COMO PREDETERMINADO----
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
  /*********ESTABLECER-PINES-DE-SALIDA*********/
  pinMode(SEN, INPUT);
  pinMode(R1, OUTPUT);
  pinMode(R2, OUTPUT);
  pinMode(R3, OUTPUT);
  pinMode(R4, OUTPUT);
  wdt_enable(WDTO_1S);
}


void loop() {
  unsigned long currentMillis = millis();

  humedad = dht.readHumidity();
  temp = dht.readTemperature();
  //Serial.print("Temperatura: ");//(OPCIONAL) IMPRIMIMOS LA TEMPERATURA A TRAVÉS DEL MONITOR SERIE
  //Serial.print(temp);//(OPCIONAL) IMPRIMIMOS LA TEMPERATURA A TRAVÉS DEL MONITOR SERIE
  //Serial.print("ºC Humedad: ");//(OPCIONAL) IMPRIMIMOS LA TEMPERATURA A TRAVÉS DEL MONITOR SERIE
  //Serial.print(humedad);//(OPCIONAL) IMPRIMIMOS LA TEMPERATURA A TRAVÉS DEL MONITOR SERIE
  //Serial.println("%");//(OPCIONAL) IMPRIMIMOS LA TEMPERATURA A TRAVÉS DEL MONITOR SERIE
  delay(1000);
  oled.clearDisplay();
  oled.setTextColor(WHITE);
  oled.setCursor(38, 5);
  oled.setTextSize(2);
  oled.print("Temp");//IMPRIMIMOS LA PALABRA TEMPERATURA A TRAVÉS DEL DISPLAY LCD

  oled.setCursor(44, 28);
  oled.setTextSize(2);
  oled.print(temp);//IMPRIMIMOS LA TEMPERATURA A TRAVÉS DEL DISPLAY LCD
  oled.print("C");
  oled.display();
  /***********BLOQUE-DE-CODIGO-PARA-IMPRIMIR-LA-DISTANCIA-A-TRAVÉS-DEL-DISPLAY-LCD***********/
  delay(50); { //TIEMPO AL SENSOR ENTRE PINGS
    distancia = sonar.ping_cm();//GUARDAMOS LA DISTANCIA MEDIDA EN LA VARIABLE
  }
  //Serial.print("Distancia: ");//(OPCIONAL) IMPRIMIMOS LA DISTANCIA A TRAVÉS DEL MONITOR SERIE
  //Serial.print(distancia); //(OPCIONAL) IMPRIMIMOS LA DISTANCIA A TRAVÉS DEL MONITOR SERIE
  delay(50);
  oled.setCursor(35, 50);
  oled.setTextSize(2);
  oled.print(distancia);//IMPRIMIMOS LA DISTANCIA A TRAVÉS DEL DISPLAY LCD (ARROJA 0 CUANDO SE SUPERA LA DISTANCIA MÁXIMA)
  oled.print("Cm");
  oled.display();
  /***********BLOQUE-DE-CODIGO-PARA-COM-BLUETOOTH***********/
  if (Serial.available() > 0) {
    DATO = Serial.read();
  }
  switch (DATO) {
    case '1' :  if (temp == 28) {
        myDFPlayer.play(4);
        Serial.print("REPRODUCIONDO AUDIO");
        delay(7000);
      }
      else if (temp == 29) {
        myDFPlayer.play(9);
        Serial.print("REPRODUCIONDO AUDIO");
        delay(7000);
      }
      else if (temp == 30) {
        myDFPlayer.play(5);
        Serial.print("REPRODUCIONDO AUDIO");
        delay(9000);
      }
      else if (temp == 31) {
        myDFPlayer.play(6);
        Serial.print("REPRODUCIONDO AUDIO");
        delay(7000);
      }

      else if (temp == 32) {
        myDFPlayer.play(7);
        Serial.print("REPRODUCIONDO AUDIO");
        delay(9000);
      }
      else if (temp == 33) {
        myDFPlayer.play(8);
        Serial.print("REPRODUCIONDO AUDIO");
        delay(7000);
      }
      else if (temp == 34) {
        myDFPlayer.play(1);
        Serial.print("REPRODUCIONDO AUDIO");
        delay(7000);
      }
      break ;

    case '2' : digitalWrite (R1, HIGH);
      break ;

    case '3' : digitalWrite (R1, LOW);
      myDFPlayer.play(2);
      break ;
  }
  /***********FIN-DEL-BLOQUE-DE-CODIGO-PARA-COM-BLUETOOTH***********/
  if (((currentMillis) - previousMillisWDT) >= intervalWDT)
  {
    previousMillisWDT = currentMillis;
    wdt_reset();
  }
}
/***********FIN-DEL-CODIGO***********/
