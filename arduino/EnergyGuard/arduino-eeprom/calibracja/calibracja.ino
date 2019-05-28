


#include <EEPROM.h>
#include <SimpleModbusMaster.h>



//------------------------------------ Modbus RTU - Begin ---------------------------------------
/*
   Function 3  - READ_HOLDING_REGISTERS
*/

//////////////////// Port information ///////////////////
#define baud 9600    // For Modbus
#define timeout 1000
#define polling 150  // the scan rate orygnally was 200

#define retry_count 10

// used to toggle the receive/transmit pin on the driver
#define TxEnablePin 2       // Arduino Leonardo occupation pin D2


// This is the easiest way to create new packets
// Add as many as you want. TOTAL_NO_OF_PACKETS
// is automatically updated.
enum
{
  PACKET1,      // Instantenous Power lines L1, L2 L3
  TOTAL_NO_OF_PACKETS // leave this last entry
};

// Create an array of Packets to be configured
Packet packets[TOTAL_NO_OF_PACKETS];

packetPointer packet1 = &packets[PACKET1];

// Data read from the arduino slave will be stored in this array
// if the array is initialized to the packet.
unsigned int readPower[6];

long  PowerL1,
      PowerL2,
      PowerL3;

//------------------------------------ Modbus RTU - End ---------------------------------------------

#define HEATER_6KW_L1   9              // 6kW heater on phase L1  (PWM signal)
#define HEATER_6KW_L2  11              // 6kW heater on phase L2  (PWM signal)
#define HEATER_6KW_L3  13              // 6kW heater on phase L3  (PWM signal)

int LinePins[] = {HEATER_6KW_L1, HEATER_6KW_L2, HEATER_6KW_L3};


//--------------
void setup() {
  //------------- setup code here, to run once ------------------


  //------------------------------------ Modbus RTU - Begin ---------------------------------------

  // The modbus packet constructor function will initialize
  // the individual packet with the assigned parameters. You can always do this
  // explicitly by using struct pointers. The first parameter is the address of the
  // packet in question. It is effectively the "this" parameter in Java that points to
  // the address of the passed object. It has the following form:
  // modbus_construct(packet, id, function, address, data, register array)


  // For functions 3 data is the number of registers


  // read registers
  modbus_construct(packet1, 1, READ_HOLDING_REGISTERS, 0x144, 6, readPower);   // Start address of Power register

  /* Initialize communication settings:  */
  modbus_configure(&Serial1, baud, SERIAL_8N2, timeout, polling, retry_count, TxEnablePin, packets, TOTAL_NO_OF_PACKETS);

  //------------------------------------ Modbus RTU - End ---------------------------------------------

  //  Initialize pins for PWM controls
  pinMode(HEATER_6KW_L1, OUTPUT);   // 6kW heater on phase L1  (PWM signal)
  pinMode(HEATER_6KW_L2, OUTPUT);   // 6kW heater on phase L2  (PWM signal)
  pinMode(HEATER_6KW_L3, OUTPUT);   // 6kW heater on phase L3  (PWM signal)


  int eeAddress, LineOffset, LineCalibContent, CalibSize;
  int i, j;
  int PWM_duty;

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


  delay(2000);                     // Wait for stability of Power Meter - 2s (soft start completed)

  do
  {
    modbus_update();            // Read data from energy meter - packet 1
  } while (!Serial1.available());

  // Modbus transmision has ended
  // Combine parameters into 32 bit format

  PowerL1 = (long) readPower[0] << 16 | (long) readPower[1];       // Read instantenous power from Energy Meter in Watts (W)
  PowerL2 = (long) readPower[2] << 16 | (long) readPower[3];
  PowerL3 = (long) readPower[4] << 16 | (long) readPower[5];


  Serial.println("Base power L1, L2, L3: ");
  Serial.print(PowerL1);
  Serial.print(" ");
  Serial.print(PowerL2);
  Serial.print(" ");
  Serial.println(PowerL3);
  Serial.println(" ");
  Serial.println(" ");

  // set PWM to max duty
  for (j = 0; j < 3; j++) {
    analogWrite(LinePins[j], 255);
  }
  delay(3000);                     // Wait for stability of Power Meter - 2s (soft start completed)

  // -------------- Read maximum Power data -----------
  // ----------------------- start ---------------------

  Serial.println("Heater On, Max Power values (L1, L2, L3): ");

  for ( i = 0; i < 5; i++) // 5 times read power for PWM = 255 (max)
  {

    delay(3000);                     // Wait for stability of Power Meter - 3s (soft start completed)

    do
    {
      modbus_update();            // Read data from energy meter - packet 1
    } while (!Serial1.available());

    // Modbus transmision has ended
    // Combine parameters into 32 bit format

    PowerL1 = (long) readPower[0] << 16 | (long) readPower[1];       // Read instantenous power from Energy Meter in Watts (W)
    PowerL2 = (long) readPower[2] << 16 | (long) readPower[3];
    PowerL3 = (long) readPower[4] << 16 | (long) readPower[5];

    CalibData_L1[i].power = (int) PowerL1;
    CalibData_L2[i].power = (int) PowerL2;
    CalibData_L3[i].power = (int) PowerL3;


    Serial.print(CalibData_L1[i].power);
    Serial.print(" ");
    Serial.print(CalibData_L2[i].power);
    Serial.print(" ");
    Serial.print(CalibData_L3[i].power);
    Serial.println(" ");
  }

  // Save max Power
  CalibData_L1[15].pwm = 255;
  CalibData_L1[15].power = (int) PowerL1;

  CalibData_L2[15].pwm = 255;
  CalibData_L2[15].power =  (int) PowerL2;

  CalibData_L3[15].pwm = 255;
  CalibData_L3[15].power =  (int) PowerL2;

  // Take min value from 5 mesueres
  for ( i = 0; i < 5; i++)
  {
    CalibData_L1[15].power = min(CalibData_L1[15].power, CalibData_L1[i].power);
    CalibData_L2[15].power = min(CalibData_L2[15].power, CalibData_L2[i].power);
    CalibData_L3[15].power = min(CalibData_L3[15].power, CalibData_L3[i].power);
  }

  // -------------- Read maximum Power data -----------
  // ----------------------- end ----------------------

  // reset PWM duty
  for (j = 0; j < 3; j++)
  {
    analogWrite(LinePins[j], 0);
    Serial.print("Pin reset: ");
    Serial.println(LinePins[j]);
  }

  Serial.println("Intermediate values of power L1, L2, L3: ");

  // set intermediate values of power
  for ( i = 0; i < 15; i++)
  {
    CalibData_L1[i].power = (int) CalibData_L1[15].power * i / 16;
    CalibData_L2[i].power = (int) CalibData_L2[15].power * i / 16;
    CalibData_L3[i].power = (int) CalibData_L3[15].power * i / 16;


    Serial.print(CalibData_L1[i].power);
    Serial.print(" ");
    Serial.print(CalibData_L2[i].power);
    Serial.print(" ");
    Serial.print(CalibData_L3[i].power);
    Serial.println(" ");
  }
  Serial.print(CalibData_L1[15].power);
  Serial.print(" ");
  Serial.print(CalibData_L2[15].power);
  Serial.print(" ");
  Serial.print(CalibData_L3[15].power);
  Serial.println(" ");

  // ---------------- Start calibration --------------
  // ----------------------- start --------------------

  // clear pwm values
  for ( i = 0; i < 15; i++)
  {
    CalibData_L1[i].pwm = 0;
    CalibData_L2[i].pwm = 0;
    CalibData_L3[i].pwm = 0;
  }

  PWM_duty = 254;

  do
  {
    Serial.print(PWM_duty);
    Serial.print(" ");

    for (j = 0; j < 3; j++)      // Set PWM duty for all lines at ones
    {
      analogWrite(LinePins[j], PWM_duty);
    }

    delay(2000);                     // Wait for stability of Power Meter - 2s

    do
    {
      modbus_update();            // Read data from energy meter - packet 1
    } while (!Serial1.available());

    // Modbus transmision has ended
    // Combine parameters into 32 bit format

    PowerL1 = (long) readPower[0] << 16 | (long) readPower[1];       // Read instantenous power from Energy Meter in Watts (W)
    PowerL2 = (long) readPower[2] << 16 | (long) readPower[3];
    PowerL3 = (long) readPower[4] << 16 | (long) readPower[5];

    Serial.print(" x");
    Serial.print(PowerL1);
    Serial.print(" x");
    Serial.print(PowerL2);
    Serial.print(" x");
    Serial.print(PowerL3);

    for ( i = 14; i > 0; i = i - 1 )
    {
      if (CalibData_L1[i].power > (int) PowerL1)
        if (CalibData_L1[i].pwm == 0 )
          CalibData_L1[i].pwm = PWM_duty;

      if (CalibData_L2[i].power > (int) PowerL2)
        if (CalibData_L2[i].pwm == 0 )
          CalibData_L2[i].pwm = PWM_duty;

      if (CalibData_L3[i].power > (int) PowerL3)
        if (CalibData_L3[i].pwm == 0 )
          CalibData_L3[i].pwm = PWM_duty;
    }

    PWM_duty = PWM_duty - 1;

    Serial.println(" next ");

  } while (PWM_duty > 0);

  // ---------------- Start calibration --------------
  // ----------------------- end ----------------------


  // Print found PWM - power pairs (calibrated data)- Before writing to eeprom


  for ( i = 0; i < 16; i++)
  {
    Serial.print("Mesured L1 (PWM - Power): ");
    Serial.print(CalibData_L1[i].pwm);
    Serial.print(" - ");
    Serial.println(CalibData_L1[i].power);
  }

  for ( i = 0; i < 16; i++)
  {
    Serial.print("Mesured L2 (PWM - Power): ");
    Serial.print(CalibData_L2[i].pwm);
    Serial.print(" - ");
    Serial.println(CalibData_L2[i].power);
  }

  for ( i = 0; i < 16; i++)
  {
    Serial.print("Mesured L3 (PWM - Power): ");
    Serial.print(CalibData_L3[i].pwm);
    Serial.print(" - ");
    Serial.println(CalibData_L3[i].power);
  }




  //
  // Coping data to EEPROM
  //


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

  // Clear var CalibData
  for ( i = 0; i < 16; i++)
  {
    CalibData_L1[i].pwm = 0;
    CalibData_L1[i].power = 0;

    CalibData_L1[i].pwm = 0;
    CalibData_L1[i].power = 0;

    CalibData_L1[i].pwm = 0;
    CalibData_L1[i].power = 0;

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

//--------------
void loop() {
  //--------------

  // put your main code here, to run repeatedly:

}
