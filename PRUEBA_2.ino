//NOMBRE DEL PROYECTO: SMART CARD
//FECHA DE CREACION: 20/11/2020
//NOMBRE DE LA COMPAÑIA: DOMOTICS ELECTRONICS.
//PROPIEDAD INTELECTUAL: DOMOTICS ELECTRONICS.
//DESAROLLADOR: EDMUNDO BARBOSA G.
//JEFE DE DISEÑO ELECTRINICO: EDMUNDO BARBOSA G.
//DURACION DEL PROJECTO: 3 MESES
//VESION: 2.1.0.0
//LIBERIAS-DOMOTICS
#include <Wire.h>//Libreria I2C
#include <NewPing.h>//Libreria para sensor de distancia
#include <DHT.h>//Libreria para sensor de temperatura
#include <EEPROM.h>
#include "SoftwareSerial.h"
#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"
#include "Sodaq_DS3231.h"//Libreria para RELOJ
#include <Adafruit_GFX.h>   //Libreria para pantallas graficas
#include <Adafruit_SSD1306.h>// libreria para controlador SSD1306
SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);
#define ANCHO 128           //Reemplaza ocurrencia de ANCHO por 128
#define ALTO 64             //Reemplaza ocurrencia de ALTO por 64
#define OLED_RESET 4        //Necesario por la libreria pero no usado
Adafruit_SSD1306 oled(ANCHO, ALTO,  &Wire, OLED_RESET);
char DiaSemana[][4] = {"Dom", "Lun", "Mar", "Mie", "Jue", "Vie", "Sab" };
struct VARIABLES {
  unsigned long START0 = 0;
  unsigned long START1 = 0;
  unsigned long START2 = 0;
  unsigned long START3 = 0;
  unsigned long START4 = 0;
  unsigned long START5 = 0;
  unsigned long STARTRELE = 0;
  unsigned long STARTRELEON = 0;
  unsigned long TIEMPOTEMP = 0;
  unsigned long STARTPANTALLA = 0;
  unsigned long STARTSENSOR = 0;
  unsigned long STARTTEX0 = 0;
  int8_t  ESTADO0 = 0;
  int8_t ESTADO1 = 0;
  int8_t ESTADO2 = 0;
  int8_t ESTADO3 = 0;
  int8_t ESTADO4 = 0;
  int8_t ESTADO5 = 0;
  int8_t ESTADORELE = 0;
  int8_t ESTADORELEON = 0;
  char DATO = 0;
};
VARIABLES VARIABLE;
const float sonido = 34300.0; // Velocidad del sonido en cm/s
////////////////////////////////////////////////////////////////
#define SEN 2// DEFINIMOS EL PIN 2 PARA EL DHT11
int8_t TEMP, HUMEDAD;//VARIABLE PARA ALMACENAR LOS DATOS DE DHT11
DHT dht (SEN, DHT11);
///////////////////////////////////////////////////////////////
#define TRIGGER_PIN  5  //TRIGGER=D5
#define ECHO_PIN     6  //ECHO=D6
#define MAX_DISTANCE 200//Maximo distancia para medir 400-500cm
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
int8_t  volatile DISTANCIA = 0;
#define R1  A0
const int8_t BUZZER = 9;
void setup() {
  Serial.begin (9600);
  Wire.begin();//INICIAR BUS I2C
  mySoftwareSerial.begin(9600);
  oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);//INICIAR PANTALLA CON DIRECCIÓN 0X3C
  dht.begin();//INICIAR COM. CON ARDUINO
  rtc.begin();
  Serial.println(F("INICIANDO EL DFPLAYER..."));
  if (!myDFPlayer.begin(mySoftwareSerial)) { //SOFTWARESERIAL PARA COMUNICARSE CON EL MP3.
    Serial.println(F("NO SE PUEDE EMPEZAR: "));
    Serial.println(F("1.¡POR FAVOR, VUELVA A COMPROBAR LA CONEXIÓN!"));
    Serial.println(F("2.¡INSERTA LA TARJETA SD!"));
    while (true);
  }
  Serial.println(F("DFPlayer Mini online."));
  myDFPlayer.setTimeOut(500); //Set serial communictaion time out 500ms
  //----AJUSTAR EL VOLUMEN----
  myDFPlayer.volume(30);  //AJUSTAR EL VOLUMEN (0~30).
  myDFPlayer.volumeUp(); //SUBIR VOLUMEN
  myDFPlayer.volumeDown(); //BAJAR VOLUMEN
  //----ESTABLECER UN ECUALIZADOR DIFERENTE----
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
  //----ESTABLECER EL DISPOSITIVO SD COMO PREDETERMINADO----
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
  pinMode (R1, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  //EEPROM.put(0, ALERTA);
}
void loop() {
  STARTULTRASONIDO();
  HUMEDAD = dht.readHumidity();
  TEMP = dht.readTemperature();
  if (Serial.available() > 0) {
    VARIABLE.DATO = Serial.read();
  }
  switch (VARIABLE.DATO) {
    case 0:
      ASISTENTE();
      PANTALLA();
      break;
    case '1':
      PANTALLA();
      TEMPERATURA();
      break;
    case '2':
      PANTALLA();
      break;
    case '3':
      PANTALLA();
      RELEON();
      break;
    case '4':
      PANTALLA();
      RELEOFF();
      break;
    case '5':
      RELOJ();
      CONFIRMACION();
      break;
    case '6':
      MEDIDOR();
      PANTALLA();
      break;
  }
}
/////////////////////////////////////////////////////////////////////////////////////////
//////////////////////FINAL-DEL-CODIGO//////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
void RELEON() {
  switch (VARIABLE.ESTADORELE) {
    case 0: digitalWrite(R1, HIGH);
      if (millis() > VARIABLE.STARTRELE + 2000) {
        myDFPlayer.play(11);
        VARIABLE.STARTRELE = millis();
        VARIABLE.ESTADORELE = 1;
      }
      break;
    case 1:
      break;
  }
}
void RELEOFF() {
  switch (VARIABLE.ESTADORELEON) {
    case 0: digitalWrite(R1, LOW);
      if (millis() > VARIABLE.STARTRELEON + 2000) {
        myDFPlayer.play(11);
        VARIABLE.STARTRELEON = millis();
        VARIABLE.ESTADORELEON = 1;
      }
      break;
    case 1:
      break;
  }
}


void ASISTENTE() {
  switch (VARIABLE.ESTADO0) {
    case 0: digitalWrite(R1, HIGH);
      VARIABLE.START0 = millis();
      VARIABLE.ESTADO0 = 1;
      break;
    case 1: if (millis() > VARIABLE.START0 + 2500) {
        myDFPlayer.play(13);
        VARIABLE.START0 = millis();
        VARIABLE.ESTADO0 = 2;
      }
      break;
    case 2:
      break;
  }
}


void TEMPERATURA() {
  switch (TEMP) {
    case 28: if (millis() > VARIABLE.TIEMPOTEMP + 3000) {
        myDFPlayer.play(5);
        VARIABLE.TIEMPOTEMP = millis();
      }
      break;
    case 29: if  (millis() > VARIABLE.TIEMPOTEMP + 3000) {
        myDFPlayer.play(10);
        VARIABLE.TIEMPOTEMP = millis();
      }
      break;
    case 30: if (millis() > VARIABLE.TIEMPOTEMP + 3000) {
        myDFPlayer.play(6);
        VARIABLE.TIEMPOTEMP = millis();
      }
      break;
    case 31: if (millis() > VARIABLE.TIEMPOTEMP + 3000) {
        myDFPlayer.play(7);
        VARIABLE.TIEMPOTEMP = millis();
      }
      break;
    case 32: if (millis() > VARIABLE.TIEMPOTEMP + 3000) {
        myDFPlayer.play(8);
        VARIABLE.TIEMPOTEMP = millis();
      }
      break;
    case 33: if (millis() > VARIABLE.TIEMPOTEMP + 3000) {
        myDFPlayer.play(9);
        VARIABLE.TIEMPOTEMP = millis();
      }
      break;
  }
}


void PANTALLA () {
  if (millis() > VARIABLE.STARTPANTALLA + 400) {
    VARIABLE.STARTPANTALLA = millis();
    oled.clearDisplay();
    oled.setTextColor(WHITE);
    oled.setCursor(38, 5);
    oled.setTextSize(2);
    oled.print("Temp");//IMPRIMIMOS LA PALABRA TEMPERATURA A TRAVÉS DEL DISPLAY LCD
    oled.setCursor(44, 28);
    oled.setTextSize(2);
    oled.print(TEMP);//IMPRIMIMOS LA TEMPERATURA A TRAVÉS DEL DISPLAY LCD
    oled.print("C");
    oled.display();
    /***********BLOQUE-DE-CODIGO-PARA-IMPRIMIR-LA-DISTANCIA-A-TRAVÉS-DEL-DISPLAY-LCD***********/
    oled.setCursor(35, 50);
    oled.setTextSize(2);
    oled.print(DISTANCIA); //IMPRIMIMOS LA DISTANCIA A TRAVÉS DEL DISPLAY LCD (ARROJA 0 CUANDO SE SUPERA LA DISTANCIA MÁXIMA)
    oled.print("Cm");
    oled.display();
  }
}


void RELOJ() {
  DateTime now = rtc.now();
  oled.clearDisplay();
  oled.setTextColor(WHITE);
  oled.setCursor(14, 6);
  oled.setTextSize(2);
  oled.print(now.year(), DEC);
  oled.print("/");
  oled.print(now.month(), DEC);
  oled.print("/");
  oled.print(now.date(), DEC);
  oled.print(" ");

  oled.setCursor(45, 26);
  oled.setTextSize(2);
  oled.print(DiaSemana[now.dayOfWeek()]);

  oled.setCursor(19, 46);
  oled.setTextSize(2);
  oled.print(now.hour(), DEC);
  oled.print(":");
  oled.print(now.minute(), DEC);
  oled.print(":");
  oled.print(now.second(), DEC);
  oled.print(" ");
  oled.display();
}

void CONFIRMACION() {
  switch (VARIABLE.ESTADO1) {
    case 0: digitalWrite(R1, HIGH);
      if ((millis() - VARIABLE.START1) >= 2000) {
        myDFPlayer.play(2);
        VARIABLE.START1 = millis();
        VARIABLE.ESTADO1 = 1;
      }
      break;
    case 1:
      break;
  }
  //Serial.println(F("FUERA DEL PELIGRO DE COLISIÓN"));
}

void MEDIDOR() {
  if  (30 <= DISTANCIA) {
    switch (VARIABLE.ESTADO2) {
      case 0:
        noTone(BUZZER);
        VARIABLE.ESTADO2 = 1;
        break;
      case 1:
        if (millis() > VARIABLE.START2 + 5000) {
          myDFPlayer.play(16);
          VARIABLE.START2 = millis();
          //Serial.print(EEPROM.get(0, ALERTA));
          Serial.print(DISTANCIA);
          Serial.print(F(", FUERA DEL PELIGRO DE COLISION"));
          Serial.println(F("  "));
        }
        VARIABLE.ESTADO2 = 0;
        break;
    }
  }
  else if (20 <= DISTANCIA ) {
    switch (VARIABLE.ESTADO3) {
      case 0:
        if (millis() > VARIABLE.START3 + 5000) {
          myDFPlayer.play(15);
          VARIABLE.START3 = millis();
          Serial.print(F("Distancia = "));
          Serial.print(DISTANCIA);
          Serial.print(F(", ALERTA DE COLISION"));
          Serial.println(F("  "));
        }
        VARIABLE.ESTADO3 = 1;
        break;
      case 1:
        VARIABLE.ESTADO3 = 0;
        tone(BUZZER, 2000, 200);
        break;
    }
  }
  else if (10 <= DISTANCIA) {
    switch (VARIABLE.ESTADO4) {
      case 0:
        if (millis() > VARIABLE.START4 + 5000) {
          myDFPlayer.play(17);
          VARIABLE.START4 = millis();
          Serial.print(F("Distancia = "));
          Serial.print(DISTANCIA);
          Serial.print(F(", ALERTA DE COLISION"));
          Serial.println(F("  "));
        }
        VARIABLE.ESTADO4 = 1;
        break;
      case 1:
        tone(BUZZER, 2500, 200);
        VARIABLE.ESTADO4 = 0;
        break;
    }
  }
  else if (1 <= DISTANCIA) {
    switch (VARIABLE.ESTADO5) {
      case 0:
        if (millis() > VARIABLE.START5 + 5000) {
          myDFPlayer.play(14);
          VARIABLE.START5 = millis();
          Serial.print(F("Distancia = "));
          Serial.print(DISTANCIA);
          Serial.print(F(", ALERTA DE COLISION"));
          Serial.println(F("  "));
        }
        VARIABLE.ESTADO5 = 1;
        break;
      case 1:
        tone(BUZZER, 3000, 200);
        VARIABLE.ESTADO5 = 0;
        break;
    }
  }
}
void STARTULTRASONIDO () {
  if (millis() > VARIABLE.STARTSENSOR + 70) {
    DISTANCIA = sonar.ping_cm();//GUARDAMOS LA DISTANCIA MEDIDA EN LA VARIABLE
    VARIABLE.STARTSENSOR = millis();
  }
}
