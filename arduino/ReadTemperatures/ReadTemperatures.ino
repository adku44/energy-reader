#include <OneWire.h>
#include <DallasTemperature.h>

#define TEMPERATURE_PRECISION  9       // Precision for dallas sensor

// Arduino Leonardo occupation pins
#define ONE_WIRE_BUS          12       // Dallas temperature sensor' bus connected to port 2 on the Arduino

#define POMP_RED_BUFFOR_CWU   5        // Hot water pomp - drinking water
#define POMP_500_BUFFOR       7        // 500 liter bufor pomp

#define HEATER_6KW_L1   6              // 6kW heater on phase L1  (PWM signal)
#define HEATER_6KW_L2  11              // 6kW heater on phase L2  (PWM signal)
#define HEATER_6KW_L3  13              // 6kW heater on phase L3  (PWM signal)
#define HEATER_4_2KW   10              // 4.2kW heater            (on/off signal)



// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// arrays to hold device addresses
DeviceAddress insideThermometer, outsideThermometer;

DeviceAddress OUTcoPCthermometer   = { 0x28, 0xDA, 0xEC, 0x2C, 0x05, 0x00, 0x00, 0xB9 };    // wyj. CO - Pompa Ciepła
DeviceAddress OUTcwuPCthermometer  = { 0x28, 0x11, 0xEE, 0x2C, 0x05, 0x00, 0x00, 0x5D };    // wyj. CWU - Pompa Ciepła
DeviceAddress Buffor300Thermometer = { 0x28, 0x7F, 0xBD, 0x2D, 0x05, 0x00, 0x00, 0xD9 };    // buffor 300l - góra
DeviceAddress OutHeatThermometer   = { 0x28, 0x39, 0x01, 0x2E, 0x05, 0x00, 0x00, 0xF8 };    // wyj. Kaloryfery
DeviceAddress InHeatThermometer    = { 0x28, 0x2E, 0xE6, 0x2C, 0x05, 0x00, 0x00, 0xAA };    // wej. Kaloryfery
DeviceAddress cwuRedTopThermometer = { 0x28, 0xD7, 0xCF, 0x2D, 0x05, 0x00, 0x00, 0xCA };    // CWU czerwony 300l - góra
DeviceAddress cwuRedBotThermometer = { 0x28, 0x74, 0xF4, 0x2D, 0x05, 0x00, 0x00, 0x3B };    // CWU czerwony 300l - dół
DeviceAddress CO500BotThermometer  = { 0x28, 0x59, 0x54, 0x2D, 0x05, 0x00, 0x00, 0xF4 };    // CO 500l  - dół
DeviceAddress CO500TopThermometer  = { 0x28, 0xEA, 0xEE, 0x2C, 0x05, 0x00, 0x00, 0xD7 };    // CO 500l  - góra
DeviceAddress OutsideThermometer   = { 0x28, 0x51, 0xFE, 0x2C, 0x05, 0x00, 0x00, 0x54 };    // temperatura zewnetrzna

// Sensor' addreses 
//1	28-0000052cecda   wyj. CO - Pompa Ciepła	Device Address: 28DAEC2C050000B9
//2	28-0000052cee11	  wyj. CWU - Pompa Ciepła	Device Address: 2811EE2C0500005D
//3	28-0000052dbd7f   buffor 300l - góra		Device Address: 287FBD2D050000D9
//4	28-0000052e0139	  wyj. Kaloryfery		Device Address: 2839012E050000F8
//5	28-0000052ce62e	  wej. Kaloryfery		Device Address: 282EE62C050000AA
//6	28-0000052dcfd7	  CWU czerwony 300l - góra	Device Address: 28D7CF2D050000CA
//7	28-0000052df474	  CWU czerwony 300l - dół	Device Address: 2874F42D0500003B
//8	28-0000052d5459	  CO 500l  - dół 		Device Address: 2859542D050000F4
//9	28-0000052ceeea	  CO 500l  - góra		Device Address: 28EAEE2C050000D7
//10    28-0000052cfe51	  temperatura zewnetrzna	Device Address: 2851FE2C05000054

// Device 0 Address: 2874F42D0500003B - CWU czerwony 300l - dół	
// Device 1 Address: 28EAEE2C050000D7 - CO 500l  - góra
// Device 2 Address: 28DAEC2C050000B9 - wyj. CO - Pompa Ciepła
// Device 3 Address: 282EE62C050000AA - wej. Kaloryfery	
// Device 4 Address: 2811EE2C0500005D - wyj. CWU - Pompa Ciepła
// Device 5 Address: 2851FE2C05000054 - temperatura zewnetrzna
// Device 6 Address: 2859542D050000F4 - CO 500l  - dół 	
// Device 7 Address: 2839012E050000F8 - wyj. Kaloryfery	
// Device 8 Address: 28D7CF2D050000CA - CWU czerwony 300l - góra
// Device 9 Address: 287FBD2D050000D9 - buffor 300l - góra
 
 
// Declaration of variables for Temperature 
float temp500top, temp500bot, temp300, tempRedTop, tempRedBot;  // water buffors
float tempOutside;                                              // outside of building
float tempHeatOut, tempHeatIn;                                  // water temperature going out/in water buffor
float tempPCoutCO, tempPCoutCWU;                                // water comming out from heating pump


 
 
void setup(void)
{
  // start serial port
  Serial.begin(9600);
  Serial.println("Dallas Temperature IC Control Reading ...");
  delay(5000);
  
  // Start up the library
  sensors.begin();

  // locate devices on the bus
  Serial.print("Locating devices...");
  Serial.print("Found ");
  // sensors.getDeviceCount();
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" devices.");

  // report parasite power requirements
  Serial.print("Parasite power is: "); 
  if (sensors.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");

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
  
  
  // Self test of Dallas devices
  // If device is not found then device index has been changed or device address
  if (!sensors.getAddress(cwuRedBotThermometer, 0)) Serial.println("Unable to find address for Device 0 - CWU czerwony 300l - dół"); 
  if (!sensors.getAddress(CO500TopThermometer,  1)) Serial.println("Unable to find address for Device 1 - CO 500l  - góra"); 
  if (!sensors.getAddress(OUTcoPCthermometer,   2)) Serial.println("Unable to find address for Device 2 - wyj. CO - Pompa Ciepła"); 
  if (!sensors.getAddress(InHeatThermometer,    3)) Serial.println("Unable to find address for Device 3 - wej. Kaloryfery"); 
  if (!sensors.getAddress(OUTcwuPCthermometer,  4)) Serial.println("Unable to find address for Device 4 - wyj. CWU - Pompa Ciepła"); 
  if (!sensors.getAddress(OutsideThermometer,   5)) Serial.println("Unable to find address for Device 5 - temperatura zewnętrzna"); 
  if (!sensors.getAddress(CO500BotThermometer,  6)) Serial.println("Unable to find address for Device 6 - CO 500l  - dół"); 
  if (!sensors.getAddress(OutHeatThermometer,   7)) Serial.println("Unable to find address for Device 7 - wyj. Kaloryfery"); 
  if (!sensors.getAddress(cwuRedTopThermometer, 8)) Serial.println("Unable to find address for Device 8 - CWU czerwony 300l - góra"); 
  if (!sensors.getAddress(Buffor300Thermometer, 9)) Serial.println("Unable to find address for Device 9 - buffor 300l - góra"); 
 
 
  // method 2: search()
  // search() looks for the next device. Returns 1 if a new address has been
  // returned. A zero might mean that the bus is shorted, there are no devices, 
  // or you have already retrieved all of them.  It might be a good idea to 
  // check the CRC to make sure you didn't get garbage.  The order is 
  // deterministic. You will always get the same devices in the same order
  //
  // Must be called before search()
  //oneWire.reset_search();
  // assigns the first address found to insideThermometer
  //if (!oneWire.search(insideThermometer)) Serial.println("Unable to find address for insideThermometer");
  // assigns the seconds address found to outsideThermometer
  //if (!oneWire.search(outsideThermometer)) Serial.println("Unable to find address for outsideThermometer");

  

  // show the addresses we found on the bus
  Serial.print("Device 0 Address: ");
  printAddress(cwuRedBotThermometer);
  Serial.println();

  Serial.print("Device 1 Address: ");
  printAddress(CO500TopThermometer);
  Serial.println();

  Serial.print("Device 2 Address: ");
  printAddress(OUTcoPCthermometer);
  Serial.println();

  Serial.print("Device 3 Address: ");
  printAddress(InHeatThermometer);
  Serial.println();
  
  Serial.print("Device 4 Address: ");
  printAddress(OUTcwuPCthermometer);
  Serial.println();

  Serial.print("Device 5 Address: ");
  printAddress(OutsideThermometer);
  Serial.println();
  
  Serial.print("Device 6 Address: ");
  printAddress(CO500BotThermometer);
  Serial.println();

  Serial.print("Device 7 Address: ");
  printAddress(OutHeatThermometer);
  Serial.println();
  
  Serial.print("Device 8 Address: ");
  printAddress(cwuRedTopThermometer);
  Serial.println();

  Serial.print("Device 9 Address: ");
  printAddress(Buffor300Thermometer);
  Serial.println();

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

  Serial.print("Device 0 Resolution: ");
  Serial.print(sensors.getResolution(cwuRedBotThermometer), DEC); 
  Serial.println();

  Serial.print("Device 1 Resolution: ");
  Serial.print(sensors.getResolution(CO500TopThermometer), DEC); 
  Serial.println();
  
  Serial.print("Device 2 Resolution: ");
  Serial.print(sensors.getResolution(OUTcoPCthermometer), DEC); 
  Serial.println();

  Serial.print("Device 3 Resolution: ");
  Serial.print(sensors.getResolution(InHeatThermometer), DEC); 
  Serial.println();

  Serial.print("Device 4 Resolution: ");
  Serial.print(sensors.getResolution(OUTcwuPCthermometer), DEC); 
  Serial.println();

  Serial.print("Device 5 Resolution: ");
  Serial.print(sensors.getResolution(OutsideThermometer), DEC); 
  Serial.println();

  Serial.print("Device 6 Resolution: ");
  Serial.print(sensors.getResolution(CO500BotThermometer), DEC); 
  Serial.println();

  Serial.print("Device 7 Resolution: ");
  Serial.print(sensors.getResolution(OutHeatThermometer), DEC); 
  Serial.println();

  Serial.print("Device 8 Resolution: ");
  Serial.print(sensors.getResolution(cwuRedTopThermometer), DEC); 
  Serial.println();

  Serial.print("Device 9 Resolution: ");
  Serial.print(sensors.getResolution(Buffor300Thermometer), DEC); 
  Serial.println();  
}

// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

// function to print the temperature for a device
void printTemperature(DeviceAddress deviceAddress)
{
  float tempC = sensors.getTempC(deviceAddress);
  Serial.print("Temp C: ");
  Serial.print(tempC);
  // Serial.print(" Temp F: ");
  // Serial.print(DallasTemperature::toFahrenheit(tempC));
}

// function to print a device's resolution
void printResolution(DeviceAddress deviceAddress)
{
  Serial.print("Resolution: ");
  Serial.print(sensors.getResolution(deviceAddress));
  Serial.println();    
}

// main function to print information about a device
void printData(DeviceAddress deviceAddress)
{
  Serial.print("Device Address: ");
  printAddress(deviceAddress);
  Serial.print(" ");
  printTemperature(deviceAddress);
  Serial.println();
}


// -------------------------------------------------------------------
// -------------------------------- Main Loop ------------------------
// -------------------------------------------------------------------


void loop(void)
{ 
  // call sensors.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures();
  Serial.println("DONE");

  // Read apparent power from Energy Meter
  Serial.print("Requesting apparent power...");
  // Read data using ModBus protocol
  Serial.println("To be DONE....");
  
 
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

  
  
   
 
  
 
 


  
  // This part is working without reading data from Power Meter



 
  
  
  // print the device information
  // printData(insideThermometer);
  // printData(outsideThermometer);
}

