#include <SimpleModbusMaster.h>

/* 
   Function 4  - READ_INPUT_REGISTERS
*/

//////////////////// Port information ///////////////////
#define baud 9600
#define timeout 1000
#define polling 300 // the scan rate bylo 200

#define retry_count 10

// used to toggle the receive/transmit pin on the driver
#define TxEnablePin 2 

#define LED 9

// This is the easiest way to create new packets
// Add as many as you want. TOTAL_NO_OF_PACKETS
// is automatically updated.
enum
{
  PACKET1,      // Apparent Power lines L1, L2 L3
  PACKET2,      // Import and export energy
  TOTAL_NO_OF_PACKETS // leave this last entry
};

// Create an array of Packets to be configured
Packet packets[TOTAL_NO_OF_PACKETS];

packetPointer packet1 = &packets[PACKET1];
packetPointer packet2 = &packets[PACKET2];


// Data read from the arduino slave will be stored in this array
// if the array is initialized to the packet.
unsigned int readPower[6];
unsigned int readEnergy[4];


float PowerL1,
      PowerL2,
      PowerL3,
      ImportEnergy,
      ExportEnergy;

void setup()
{
  // The modbus packet constructor function will initialize
  // the individual packet with the assigned parameters. You can always do this
  // explicitly by using struct pointers. The first parameter is the address of the
  // packet in question. It is effectively the "this" parameter in Java that points to
  // the address of the passed object. It has the following form:
  // modbus_construct(packet, id, function, address, data, register array)
  
 
  // For functions 4 data is the number of registers

  
  // read registers  
  modbus_construct(packet1, 1, READ_INPUT_REGISTERS, 0x0C, 6, readPower);   // Start address of Power register
  modbus_construct(packet2, 1, READ_INPUT_REGISTERS, 0x48, 4, readEnergy);  // Start address of Energy register
 
  
  ///// Starting serial for Leonardo
  Serial.begin(9600);  
  while (!Serial) { ; }  // For debugging
    
    
    
  /* Initialize communication settings:  */
 
  modbus_configure(&Serial1, baud, SERIAL_8N2, timeout, polling, retry_count, TxEnablePin, packets, TOTAL_NO_OF_PACKETS);
  
  pinMode(LED, OUTPUT);
}


float ExtractFloat(unsigned int HiByte, unsigned int LoByte)
{
    unsigned long inttemp;   // temporary value for calculations   
    float RegisterValue = 0;
    
    inttemp =  HiByte;
    inttemp =  inttemp << 16 | LoByte;

    memcpy( &RegisterValue, &inttemp, 8);
    // RegisterValue = *(float *)&inttemp;

    return RegisterValue;
}




void loop()
{
  
  modbus_update();
 
  
  if (Serial1.available())
  {   
    
    // Read parameter in float format
    ImportEnergy = ExtractFloat(readEnergy[0], readEnergy[1]);
    ExportEnergy = ExtractFloat(readEnergy[2], readEnergy[3]);
    
    PowerL1 = ExtractFloat(readPower[0], readPower[1]);
    PowerL2 = ExtractFloat(readPower[2], readPower[3]);
    PowerL3 = ExtractFloat(readPower[4], readPower[5]);

    Serial.print("Power L1: ");
    Serial.print(PowerL1);
    Serial.println(" W");
    Serial.print("Power L2: ");
    Serial.print(PowerL2);
    Serial.println(" W");
    Serial.print("Power L3: ");
    Serial.print(PowerL3);
    Serial.println(" W");
    
    Serial.print("Imported Energy: ");
    Serial.print(ImportEnergy);
    Serial.println(" kWh");
    Serial.print("Exported Energy: ");
    Serial.print(ExportEnergy);
    Serial.println(" kWh");
    Serial.println();
    
    Serial.println("Power requests");
    Serial.print("total requests:      ");
    Serial.println(packet1->requests);
    Serial.print("successful requests: ");
    Serial.println(packet1->successful_requests);
    Serial.print("failed requests:     ");
    Serial.println(packet1->failed_requests);
    Serial.print("exception requests:  ");
    Serial.println(packet1->exception_errors);
    Serial.println();
    
    Serial.println("Energy requests");
    Serial.print("total requests:      ");
    Serial.println(packet2->requests);
    Serial.print("successful requests: ");
    Serial.println(packet2->successful_requests);
    Serial.print("failed requests:     ");
    Serial.println(packet2->failed_requests);
    Serial.print("exception requests:  ");
    Serial.println(packet2->exception_errors);

    Serial.println();
    Serial.println();
  }  
  
  // analogWrite(LED, readRegs[0]>>2); // constrain adc value from the arduino slave to 255
  
  /* You can check or alter the internal counters of a specific packet like this:
     packet1->requests;
     packet1->successful_requests;
     packet1->failed_requests;
     packet1->exception_errors;
     packet2->requests;
     packet2->successful_requests;
     packet2->failed_requests;
     packet2->exception_errors;
  */
}
