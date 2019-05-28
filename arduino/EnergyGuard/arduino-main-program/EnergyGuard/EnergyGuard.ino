#include <SimpleModbusMaster.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <EEPROM.h>


//------------------------------------ Modbus RTU - Begin ---------------------------------------
/*
   Function 3  - READ_HOLDING_REGISTERS
*/

//////////////////// Port information ///////////////////
#define baud 9600    // For Modbus
#define timeout 1000
#define polling 600  // interval between reads on modbus, the scan rate orygnally was 200

#define retry_count 10

// used to toggle the receive/transmit pin on the driver
#define TxEnablePin 2       // Arduino Leonardo occupation pin D2


// This is the easiest way to create new packets
// Add as many as you want. TOTAL_NO_OF_PACKETS
// is automatically updated.
enum
{
  PACKET1,      // Apparent Power lines L1, L2 L3
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


// Arduino Leonardo occupation pins
#define ONE_WIRE_BUS   12        // Dallas temperature sensor' bus connected to port 12 on the Arduino

#define PUMP_CWU_PIN   	5        // Hot water pump, red buffer - potable water
#define PUMP_500_PIN    6        // 500 liter buffer pump

#define HEATER_6KW_L1   9              // 6kW heater on phase L1  (PWM signal)
#define HEATER_6KW_L2  11              // 6kW heater on phase L2  (PWM signal)
#define HEATER_6KW_L3  13              // 6kW heater on phase L3  (PWM signal)
#define HEATER_4_2KW   10              // 4.2kW heater            (on/off signal)

#define POWER_MARGIN  150             // minimal power margin for power meter precision (in Watt)

//------------------------------------- Dallas Temperature - Begin ----------------------------------


#define TEMPERATURE_PRECISION  9       // Precision for dallas sensor


// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// arrays to hold device addresses

DeviceAddress OUTcoPCthermometer   = { 0x28, 0xDA, 0xEC, 0x2C, 0x05, 0x00, 0x00, 0xB9 };    // wyj. CO - Pumpa Ciepła
DeviceAddress OUTcwuPCthermometer  = { 0x28, 0x11, 0xEE, 0x2C, 0x05, 0x00, 0x00, 0x5D };    // wyj. CWU - Pumpa Ciepła
DeviceAddress Buffor300Thermometer = { 0x28, 0x7F, 0xBD, 0x2D, 0x05, 0x00, 0x00, 0xD9 };    // buffor 300l - góra
DeviceAddress OutHeatThermometer   = { 0x28, 0x39, 0x01, 0x2E, 0x05, 0x00, 0x00, 0xF8 };    // wyj. Kaloryfery
DeviceAddress InHeatThermometer    = { 0x28, 0x2E, 0xE6, 0x2C, 0x05, 0x00, 0x00, 0xAA };    // wej. Kaloryfery
DeviceAddress cwuRedTopThermometer = { 0x28, 0xD7, 0xCF, 0x2D, 0x05, 0x00, 0x00, 0xCA };    // CWU czerwony 300l - góra
DeviceAddress cwuRedBotThermometer = { 0x28, 0x74, 0xF4, 0x2D, 0x05, 0x00, 0x00, 0x3B };    // CWU czerwony 300l - dół
DeviceAddress CO500BotThermometer  = { 0x28, 0xEA, 0xEE, 0x2C, 0x05, 0x00, 0x00, 0xD7 };    // CO 500l  - dół
DeviceAddress CO500TopThermometer  = { 0x28, 0x59, 0x54, 0x2D, 0x05, 0x00, 0x00, 0xF4 };    // CO 500l  - góra
DeviceAddress OutsideThermometer   = { 0x28, 0x51, 0xFE, 0x2C, 0x05, 0x00, 0x00, 0x54 };    // temperatura zewnetrzna

// Sensor' addreses
//1	28-0000052cecda   wyj. CO - Pumpa Ciepła	Device Address: 28DAEC2C050000B9
//2	28-0000052cee11	  wyj. CWU - Pumpa Ciepła	Device Address: 2811EE2C0500005D
//3	28-0000052dbd7f   buffor 300l - góra		Device Address: 287FBD2D050000D9
//4	28-0000052e0139	  wyj. Kaloryfery		Device Address: 2839012E050000F8
//5	28-0000052ce62e	  wej. Kaloryfery		Device Address: 282EE62C050000AA
//6	28-0000052dcfd7	  CWU czerwony 300l - góra	Device Address: 28D7CF2D050000CA
//7	28-0000052df474	  CWU czerwony 300l - dół	Device Address: 2874F42D0500003B
//8	28-0000052ceeea   CO 500l  - dół 		Device Address: 28EAEE2C050000D7
//9	28-0000052d5459	  CO 500l  - góra		Device Address: 2859542D050000F4
//10    28-0000052cfe51	  temperatura zewnetrzna	Device Address: 2851FE2C05000054

// Device 0 Address: 2874F42D0500003B - CWU czerwony 300l - dół
// Device 1 Address: 2859542D050000F4 - CO 500l  - góra
// Device 2 Address: 28DAEC2C050000B9 - wyj. CO - Pumpa Ciepła
// Device 3 Address: 282EE62C050000AA - wej. Kaloryfery
// Device 4 Address: 2811EE2C0500005D - wyj. CWU - Pumpa Ciepła
// Device 5 Address: 2851FE2C05000054 - temperatura zewnetrzna
// Device 6 Address: 28EAEE2C050000D7 - CO 500l  - dół
// Device 7 Address: 2839012E050000F8 - wyj. Kaloryfery
// Device 8 Address: 28D7CF2D050000CA - CWU czerwony 300l - góra
// Device 9 Address: 287FBD2D050000D9 - buffor 300l - góra


// Declaration of variables for Temperature
float temp500top, temp500bot, temp300, tempRedTop, tempRedBot;  // water buffors
float tempOutside;                                              // outside of building
float tempHeatOut, tempHeatIn;                                  // water temperature going out/in water buffor
float tempPCoutCO, tempPCoutCWU;                                // water comming out from heating pump

float prev_temp500top, prev_temp500bot, prev_temp300, prev_tempRedTop, prev_tempRedBot;  // water buffors
//float prev_tempOutside;                                              // outside of building
float prev_tempHeatOut, prev_tempHeatIn;                                  // water temperature going out/in water buffor
float prev_tempPCoutCO, prev_tempPCoutCWU;                                // water comming out from heating pump


//------------------------------------- Dallas Temperature - End ------------------------------------


int           Power_Regulator_L1,   // Power for Regulator in Watts
              Power_Regulator_L2,   // Power for Regulator in Watts
              Power_Regulator_L3,   // Power for Regulator in Watts
              CalibrationPWMDuty,
              Pump_CWU, // Pump CWU on/off
              Pump_500, // Pump buffer 500 l on/off
              SSR_3p;   // 3-phase SSR  on/off


// Variables for eeprom

int 	eeAddress, LineOffset, LineCalibContent, CalibSize;

int   i, j;   	// Loops
int   PWM_duty;   // PWM duty from 0 to 255
int   isNoon;		// noon / night

typedef struct  {
  byte pwm;			// PWM duty from 0 to 255
  int power;			// Power in Watt

} type_CalibData;

type_CalibData CalibData_L1[16];
type_CalibData CalibData_L2[16];
type_CalibData CalibData_L3[16];
type_CalibData CalibData_temp;


// Variables for sending data to serial

int Ndata;
String str_payLoad;


//--------------
void setup()
//--------------
{


  // Variables for eeprom

  LineCalibContent = sizeof(CalibData_L1);
  CalibSize = sizeof(CalibData_temp) ;


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

  Serial.begin(9600);    ///// Starting serial for Leonardo for output to Raspberry Pi

  /* Initialize communication settings:  */
  modbus_configure(&Serial1, baud, SERIAL_8N2, timeout, polling, retry_count, TxEnablePin, packets, TOTAL_NO_OF_PACKETS);

  //------------------------------------ Modbus RTU - End ---------------------------------------------

  //------------------------------------- Dallas Temperature - Begin ----------------------------------


  // Start up the library
  sensors.begin();

  // locate devices on the bus

  // assign address manually.  the addresses below will beed to be changed
  // to valid device addresses on your bus.  device address can be retrieved
  // by using either oneWire.search(deviceAddress) or individually via
  // sensors.getAddress(deviceAddress, index)
  //insideThermometer = { 0x28, 0x1D, 0x39, 0x31, 0x2, 0x0, 0x0, 0xF0 };
  //outsideThermometer   = { 0x28, 0x3F, 0x1C, 0x31, 0x2, 0x0, 0x0, 0x2 };

  // search for devices on the bus and assign based on an index.  ideally,
  // you would do this to initially discover addresses on the bus and then
  // use those addresses and manually assign them (see above) once you know
  // the devices on your bus (and assuming they don't change).
  //
  // method 1: by index

  // set the resolution to 9 bit
  sensors.setResolution(cwuRedBotThermometer, TEMPERATURE_PRECISION);
  sensors.setResolution(CO500TopThermometer , TEMPERATURE_PRECISION);
  sensors.setResolution(OUTcoPCthermometer  , TEMPERATURE_PRECISION);
  sensors.setResolution(InHeatThermometer   , TEMPERATURE_PRECISION);
  sensors.setResolution(OUTcwuPCthermometer , TEMPERATURE_PRECISION);
  sensors.setResolution(OutsideThermometer  , TEMPERATURE_PRECISION);
  sensors.setResolution(CO500BotThermometer , TEMPERATURE_PRECISION);
  sensors.setResolution(OutHeatThermometer  , TEMPERATURE_PRECISION);
  sensors.setResolution(cwuRedTopThermometer, TEMPERATURE_PRECISION);
  sensors.setResolution(Buffor300Thermometer, TEMPERATURE_PRECISION);


  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  sensors.requestTemperatures();

  // Get temperatures
  prev_temp500top   = sensors.getTempC(CO500TopThermometer);
  prev_temp500bot   = sensors.getTempC(CO500BotThermometer);
  prev_temp300      = sensors.getTempC(Buffor300Thermometer);
  prev_tempRedTop   = sensors.getTempC(cwuRedTopThermometer);
  prev_tempRedBot   = sensors.getTempC(cwuRedBotThermometer);

  //    prev_tempOutside  = sensors.getTempC(OutsideThermometer);

  prev_tempHeatOut  = sensors.getTempC(OutHeatThermometer);
  prev_tempHeatIn   = sensors.getTempC(InHeatThermometer);
  prev_tempPCoutCO  = sensors.getTempC(OUTcoPCthermometer);
  prev_tempPCoutCWU = sensors.getTempC(OUTcwuPCthermometer);

  //------------------------------------- Dallas Temperature - End ------------------------------------

  // Initialize variables

  Power_Regulator_L1 = 0;   // Power for Regulator in Watts
  Power_Regulator_L2 = 0;   // Power for Regulator in Watts
  Power_Regulator_L3 = 0;   // Power for Regulator in Watts

  Pump_CWU = 0; // Pump CWU - off
  Pump_500 = 0; // Pump buffer 500 l - off
  SSR_3p = 0;   // 3-phase SSR - off

  isNoon = 0;	// not noon / night time
  Ndata = 19; // Number of data send to Raspberry Pi
  str_payLoad = "";

  //  Initialize pins for PWM and Pumps controls
  pinMode(HEATER_6KW_L1, OUTPUT);   // 6kW heater on phase L1  (PWM signal)
  pinMode(HEATER_6KW_L2, OUTPUT);   // 6kW heater on phase L2  (PWM signal)
  pinMode(HEATER_6KW_L3, OUTPUT);   // 6kW heater on phase L3  (PWM signal)

  pinMode(HEATER_4_2KW, OUTPUT);          // 3-phase SSR

  pinMode(PUMP_CWU_PIN, OUTPUT);   	// Hot water pump - drinking water)
  pinMode(PUMP_500_PIN, OUTPUT);    // 500 liter bufor pump




  //
  // ----------------   Read from EEPROM  --------------------------------------------
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

}

#include "EnergyGuard1.h"



/////////////////////////////////
void loop()
////////////////////////////////

{

  //------------------------------------ Modbus RTU - Begin ---------------------------------------------

  delay(2000);                    // Wait for stability of Power Meter

  do
  {
    modbus_update();            // Read data from energy meter - packet 1

  } while (!Serial1.available());



  // Modbus transmision has ended
  // Combine parameters into 32 bit format

  PowerL1 = (long) readPower[0] << 16 | (long) readPower[1];       // Read apparent power from Energy Meter in Watts (W)
  PowerL2 = (long) readPower[2] << 16 | (long) readPower[3];
  PowerL3 = (long) readPower[4] << 16 | (long) readPower[5];

  //------------------------------------ Modbus RTU - End ---------------------------------------------


  //------------------------------------- Dallas Temperature - Begin ----------------------------------

  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  sensors.requestTemperatures();

  // Get temperatures
  temp500top   = sensors.getTempC(CO500TopThermometer);
  temp500bot   = sensors.getTempC(CO500BotThermometer);
  temp300      = sensors.getTempC(Buffor300Thermometer);
  tempRedTop   = sensors.getTempC(cwuRedTopThermometer);
  tempRedBot   = sensors.getTempC(cwuRedBotThermometer);

  tempOutside  = sensors.getTempC(OutsideThermometer);

  tempHeatOut  = sensors.getTempC(OutHeatThermometer);
  tempHeatIn   = sensors.getTempC(InHeatThermometer);
  tempPCoutCO  = sensors.getTempC(OUTcoPCthermometer);
  tempPCoutCWU = sensors.getTempC(OUTcwuPCthermometer);

  temp500top   = constrain(temp500top, 0, 100);
  temp500bot   = constrain(temp500bot, 0, 100);
  temp300      = constrain(temp300, 0, 100);
  tempRedTop   = constrain(tempRedTop, 0, 100);
  tempRedBot   = constrain(tempRedBot, 0, 100);
  tempHeatOut  = constrain(tempHeatOut, 0, 100);
  tempHeatIn   = constrain(tempHeatIn, 0, 100);
  tempPCoutCO  = constrain(tempPCoutCO, 0, 100);
  tempPCoutCWU = constrain(tempPCoutCWU, 0, 100);

  if (temp500top == 0)                            // remove instability in reading temperature
    temp500top   =    prev_temp500top;
  if (temp500bot == 0)
    temp500bot   =    prev_temp500bot;
  if (temp300 == 0)
    temp300      =    prev_temp300;
  if (tempRedTop == 0)
    tempRedTop   =    prev_tempRedTop;
  if (tempRedBot == 0)
    tempRedBot   =    prev_tempRedBot;
  if (tempHeatOut == 0)
    tempHeatOut  =    prev_tempHeatOut;
  if (tempHeatIn == 0)
    tempHeatIn   =    prev_tempHeatIn;
  if (tempPCoutCO == 0)
    tempPCoutCO  =    prev_tempPCoutCO;
  if (tempPCoutCWU == 0)
    tempPCoutCWU =    prev_tempPCoutCWU;

  prev_temp500top   = temp500top;
  prev_temp500bot   = temp500bot;
  prev_temp300      = temp300;
  prev_tempRedTop   = tempRedTop;
  prev_tempRedBot   = tempRedBot;
  prev_tempHeatOut  = tempHeatOut;
  prev_tempHeatIn   = tempHeatIn;
  prev_tempPCoutCO  = tempPCoutCO;
  prev_tempPCoutCWU = tempPCoutCWU;

  //------------------------------------- Dallas Temperature - End ------------------------------------




  //------------------------------------- Heating Model - Begin ---------------------------------------



  // Decision about power supply for heating system base on surplus apparent power.


  Power_Regulator_L1 = CalculatePowerForHeater (PowerL1, Power_Regulator_L1, CalibData_L1[15]);           // Watts for Power Regulator on line L1
  Power_Regulator_L2 = CalculatePowerForHeater (PowerL2, Power_Regulator_L2, CalibData_L2[15]);
  Power_Regulator_L3 = CalculatePowerForHeater (PowerL3, Power_Regulator_L3, CalibData_L3[15]);

/*
  SSR_3p = RunSSR3p (Power_Regulator_L1, Power_Regulator_L2, Power_Regulator_L3, SSR_3p);

  if ( SSR_3p == 1 )    //  3-phase SSR is switched on
  {
    Power_Regulator_L1 = Power_Regulator_L1 - 1500 ;     // Decrease power on regulator
    Power_Regulator_L2 = Power_Regulator_L2 - 1500 ;
    Power_Regulator_L3 = Power_Regulator_L3 - 1500 ;
  }
*/

  //--- Set calculated parameters on Arduino output pins to control Power Regulator

  RunPowerRegulator(HEATER_6KW_L1, PowerMappingToPWMDuty(Power_Regulator_L1, CalibData_L1) );          // phase L1
  RunPowerRegulator(HEATER_6KW_L2, PowerMappingToPWMDuty(Power_Regulator_L2, CalibData_L2) );          // phase L2
  RunPowerRegulator(HEATER_6KW_L3, PowerMappingToPWMDuty(Power_Regulator_L3, CalibData_L3) );          // phase L3


  Pump_500 = RunPomp500 (PUMP_500_PIN, (int) temp300, (int) temp500top, Pump_500); 					// Run pump of 500 l buffer


  Pump_CWU = RunPompCWU (PUMP_CWU_PIN, (int) tempPCoutCWU, (int) tempRedTop, Pump_CWU);           // Run pump CWU (300 l + 160 l)

  // if (isNoon == 1)
  // {
  //   digitalWrite(PUMP_CWU_PIN, HIGH);  			// Run CWU pump at noon (13 o'clock - whole hour)
  //   Pump_CWU = 1;
  // }
  // else
  // {
  //   digitalWrite(PUMP_CWU_PIN, LOW);
  //   Pump_CWU = 0;
  // }


  //------------------------------------- Heating Model - End -----------------------------------------
  // Decision about pump control base on temperatures



  //        if (tempOutside < 15)          // Winter mode - heating system is working


  // Output to database located on Raspberry Pi

  str_payLoad = (String) tempPCoutCO;                     // t1 Temp. wyj. CO - Pumpa Ciepła
  str_payLoad += " " + (String) tempPCoutCWU;             // t2 Temp. wyj. CWU - Pumpa Ciepła
  str_payLoad += " " + (String) temp300;                  // t3 Temp. buffor 300l - góra
  str_payLoad += " " + (String) tempHeatOut;              // t4 Temp. wyj. Kaloryfery
  str_payLoad += " " + (String) tempHeatIn;               // t5 Temp. wej. Kaloryfery
  str_payLoad += " " + (String) tempRedTop;               // t6 Temp. CWU czerwony 300l - góra
  str_payLoad += " " + (String) tempRedBot;               // t7 Temp. CWU czerwony 300l - dół
  str_payLoad += " " + (String) temp500bot;               // t8 Temp. CO 500l  - dół
  str_payLoad += " " + (String) temp500top;               // t9 Temp. CO 500l  - góra
  str_payLoad += " " + (String) tempOutside;              // t10 Temp. temperatura zewnetrzna
  str_payLoad += " " + (String) Pump_CWU;                     // Pump CWU on/off
  str_payLoad += " " + (String) Pump_500;                     // Pump buffer 500l on/off
  str_payLoad += " " + (String) PowerL1;               // L1 Aparent Power in Watts
  str_payLoad += " " + (String) PowerL2;               // L2 Aparent Power in Watts
  str_payLoad += " " + (String) PowerL3;               // L3 Aparent Power in Watts
  str_payLoad += " " + (String) Power_Regulator_L1;        // PWM signal for L1 in Watts
  str_payLoad += " " + (String) Power_Regulator_L2;        // PWM signal for L2 in Watts
  str_payLoad += " " + (String) Power_Regulator_L3;        // PWM signal for L3 in Watts
  str_payLoad += " " + (String) isNoon;                    // 3-phase SSR on/off --SSR_3p
  str_payLoad += " ";
  str_payLoad += "\n";                                     // end of string


  Serial.print(str_payLoad);

  delay(100);                        // Wait for raspberry respose


  if (Serial.available())
  {
    isNoon = Serial.read() - '0';      // Read data from Raspberry - noon time - 1 p.m. (1) or not (0)
  }

  if (isNoon < 0)
    isNoon = 0;            // Reset noon time where communication with raspberry is not present or no data has been received



}  // End loop




// ------------------------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------------------------

