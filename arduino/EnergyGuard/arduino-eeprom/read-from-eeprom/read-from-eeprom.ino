
#include <EEPROM.h>

void setup() {
  // put your setup code here, to run once:

  int eeAddress, CalibSize;

  typedef struct  {
    byte pwm;
    int power;
  } type_CalibData;

  type_CalibData CalibDataPair;

  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  CalibSize = sizeof(CalibDataPair) ;

  Serial.println("Read from EEPROM (PWM - Power): ");


  for (eeAddress = 0 ; eeAddress <  EEPROM.length(); eeAddress += CalibSize)      //Move to next address of structure
  {

    EEPROM.get(eeAddress, CalibDataPair);
    
    Serial.print(CalibDataPair.pwm);
    Serial.print(" - ");
    Serial.println(CalibDataPair.power);

  }

  Serial.println("End of EEPROM");
  Serial.println("Last EEPROM address: ");
  Serial.print(eeAddress);





}

void loop() {
  // put your main code here, to run repeatedly:

}
