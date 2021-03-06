/*Inclusion des librairies */

#include "SFE_BMP180.h"
#include "CapteurCV3F.h"
#include "Inimeac.h"
#include <Wire.h>

/* Creation d'objets */

Inimeac trame;
SFE_BMP180 bmp180;
CapteurCV3F capteurCV3F;

/*Creation des definitions*/

#define ALTITUDE 1655.0

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600);
  Serial1.begin(9600);
  bmp180.begin();
}

void loop() {
     char status;
     double T,P,H;
     String Te,Pa,He;
     int reading;
     char* pPa=NULL;
     char* pTe=NULL;
     char* pHe=NULL;
     
     status = bmp180.startTemperature();
     delay(status);
     status = bmp180.getTemperature(T);
     if (status != 0){
        status = bmp180.startPressure(3);
        delay(status);
        status = bmp180.getPressure(P,T);
        if (status != 0){
          Serial.print("Pression : ");
          Pa=String(P,2); 
          pPa = new char [Pa.length()+1];
          strcpy (pPa, Pa.c_str());
          Serial.print(pPa);
          Serial.println(" mBar (Hpa) ");
        } 
     }
      Serial.print("La vitesse du vent est de : ");
      Serial.print(capteurCV3F.getVitesseVent());
      Serial.println(" Kts");
      Serial.print("La direction du vent est de : ");
      Serial.print(capteurCV3F.getDirectionVent());
      Serial.println(char(176));
      
      Wire.beginTransmission(0x27); //adresse I2C de la serie de capteur HIH8000
      Wire.write(byte(0x00));       // Initialisation des mesures par l'envoie de zero bits 
      Wire.endTransmission();       //arret de la transmission
      
      delay(100);

      Wire.requestFrom(0x27, 4); 
      if (4 <= Wire.available())    // Faire si les 4 bits sont reçue
      { 
          /* Humidity is located in first two bytes */
        reading = Wire.read();  // receive high byte (overwrites previous reading)
        reading = reading << 8;// shift high byte to be high 8 bits
        reading |= Wire.read(); // receive low byte as lower 8 bits
        H = reading / 16382.0 * 100.0; //humidity conversion as per the documentation
        Serial.print("Humidite : ");
        He=String(H,2);
        pHe = new char [He.length()+1];
        strcpy (pHe, He.c_str());
        Serial.print(pHe);
        Serial.println ("%");
        reading = 0;//initilizing the reading to read the temperature values
        /* Temperature is located in next two bytes, padded by two trailing bits */
        reading = Wire.read();
        reading = reading << 6;
        reading |= (Wire.read()>>2);
        T = reading / 16382.0 * 165.0 - 40;//temperature conversion as per documentations...
        Serial.print("Temperature : ");
        Te=String(T,2);
        pTe = new char [Te.length()+1];
        strcpy (pTe, Te.c_str());
        Serial.print(pTe);
        Serial.print ((char(176)));
        Serial.println ("C\n\n");
        
      }
      
      trame.setTrame(capteurCV3F.getVitesseVent(),capteurCV3F.getDirectionVent(),pTe,pHe,pPa,"Y");
      char* trame1=trame.getTrame();
     // Serial2.print("Je ne comprend pas pourquoi ma trame est inverser Je ne comprend pas pourquoi");
    // Envoie de la trame a l'afficheur et site web

      Serial1.println(trame1);  // Envoie de la trame a la tablette
      Serial.println(trame1);  // Envoie de la trame au moniteur
      
      delete pPa;
      delete pTe;
      delete pHe;
}
