
#include <EEPROM.h>

void setup() {
  // put your setup code here, to run once:

  int eeAddress, LineOffset, LineCalibContent, CalibSize;
  int i;

  typedef struct  {
    byte pwm;
    int power;
  } type_CalibData;

  type_CalibData CalibData_L1[16];
  type_CalibData CalibData_L2[16];
  type_CalibData CalibData_L3[16];
  type_CalibData CalibData_temp;

  LineCalibContent = sizeof(CalibData_L1) ;
  CalibSize = sizeof(CalibData_temp) ;

  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println(CalibSize);
  Serial.println(LineCalibContent);
  
  // Initialize data



  for ( i = 0; i < 16; i++)
  {

    CalibData_L1[i].pwm = i;
    CalibData_L1[i].power = LineCalibContent - i;

    CalibData_L2[i].pwm = i;
    CalibData_L2[i].power =  2 * LineCalibContent - i;

    CalibData_L3[i].pwm = i;
    CalibData_L3[i].power =  3 * LineCalibContent - i;

  }

  //
  // Print initial data
  //


  for ( i = 0; i < 16; i++)
  {
    Serial.print("Initial Data L1 (PWM - Power): ");
    Serial.print(CalibData_L1[i].pwm);
    Serial.print(" - ");
    Serial.println(CalibData_L1[i].power);
  }

  for ( i = 0; i < 16; i++)
  {
    Serial.print("Initial Data L2 (PWM - Power): ");
    Serial.print(CalibData_L2[i].pwm);
    Serial.print(" - ");
    Serial.println(CalibData_L2[i].power);
  }

  for ( i = 0; i < 16; i++)
  {
    Serial.print("Initial Data L3 (PWM - Power): ");
    Serial.print(CalibData_L3[i].pwm);
    Serial.print(" - ");
    Serial.println(CalibData_L3[i].power);
  }

  //
  // Coping data to EEPROM
  //


  // LineCalibContent = CalibSize * 16;    // Power resolution: 16 steps

  for (LineOffset = 0;  LineOffset < 3;   LineOffset += 1)       // For each Phase Line (L1, L2, L3)
  {
    i = 0;

    for (eeAddress = LineOffset * LineCalibContent ; eeAddress <  ((LineOffset + 1) * LineCalibContent); eeAddress += CalibSize )    //Move to next address of structure
    {

      if ( LineOffset == 0 )
        EEPROM.put(eeAddress, CalibData_L1[i]);

      if ( LineOffset == 1 )
        EEPROM.put(eeAddress, CalibData_L2[i]);

      if ( LineOffset == 2 )
        EEPROM.put(eeAddress, CalibData_L3[i]);


      i++;
    }
  }

  //
  // clear calib data tables
  //

  for ( int i = 0; i < 16; i++)
  {

    CalibData_L1[i].pwm = 0;
    CalibData_L1[i].power = 0;

    CalibData_L2[i].pwm = 0;
    CalibData_L2[i].power =  0;

    CalibData_L3[i].pwm = 0;
    CalibData_L3[i].power =  0;

  }


  //
  // Read from EEPROM and print
  //

  for (LineOffset = 0;  LineOffset < 3;   LineOffset += 1)       // For each Phase Line (L1, L2, L3)
  {
    i = 0;

    for (eeAddress = LineOffset * LineCalibContent ; eeAddress <  ((LineOffset + 1) * LineCalibContent); eeAddress += CalibSize)    //Move to next address of structure
    {

      if ( LineOffset == 0 )
        EEPROM.get(eeAddress, CalibData_L1[i]);

      if ( LineOffset == 1 )
        EEPROM.get(eeAddress, CalibData_L2[i]);

      if ( LineOffset == 2 )
        EEPROM.get(eeAddress, CalibData_L3[i]);

      i++;
    }
  }

  // Print found PWM - power pairs (calibrated data)


  for ( i = 0; i < 16; i++)
  {
    Serial.print("Read from EEPROM L1 (PWM - Power): ");
    Serial.print(CalibData_L1[i].pwm);
    Serial.print(" - ");
    Serial.println(CalibData_L1[i].power);
  }

  for ( i = 0; i < 16; i++)
  {
    Serial.print("Read from EEPROM L2 (PWM - Power): ");
    Serial.print(CalibData_L2[i].pwm);
    Serial.print(" - ");
    Serial.println(CalibData_L2[i].power);
  }

  for ( i = 0; i < 16; i++)
  {
    Serial.print("Read from EEPROM L3 (PWM - Power): ");
    Serial.print(CalibData_L3[i].pwm);
    Serial.print(" - ");
    Serial.println(CalibData_L3[i].power);
  }


  Serial.print("Last EEPROM address: ");
  Serial.println(eeAddress);




}

void loop() {
  // put your main code here, to run repeatedly:

}
