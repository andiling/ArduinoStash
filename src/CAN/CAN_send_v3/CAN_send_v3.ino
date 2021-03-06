// demo: CAN-BUS Shield, send data
// Use http://www.seeedstudio.com/wiki/CAN-BUS_Shield if need help
#include <mcp_can.h>
#include <SPI.h>
#include "MD5.h"
#include "can_constants_v2.h"

// the cs pin of the version after v1.1 is default to D9
// v0.9b and v1.0 is default D10
const int SPI_CS_PIN = 9;

// Variables
uint32_t id = 0x100;
uint8_t dlc = 8;
unsigned char stmp[8] = {0, 1, 2, 3, 4, 5, 6, 7};

MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin

void setup()
{
    randomSeed(analogRead(0));
    Serial.begin(115200); // serial interface baud rate 115200 (typical rate for CAN)

    while (CAN_OK != CAN.begin(CAN_500KBPS))              // init can bus : baudrate = 500k
    {
        Serial.println("CAN BUS Shield init fail");
        delay(10);
        Serial.println(" Init CAN BUS Shield again");
    }
    Serial.println("CAN BUS Shield init ok!");
}

void loop()
{
    // Check if user has inputted a message
    if (Serial.available())
    {
        id = Serial.parseInt();
        dlc = Serial.parseInt();
        for(int i = 0; i < dlc; i++)
            stmp[i] = Serial.parseInt();
    }
    else
    {
        // Otherwise, Generate a random CAN message
        GenerateMessage(id, dlc, &stmp[0]);
    }
    // Display message on Serial interface
    PrintMessage();
   
    // send data:  id = 0x00, standrad frame, data len = 8, stmp: data buf
    CAN.sendMsgBuf(id, 0, dlc, stmp);
    SendAuthMessages();
    delay(1000); 
}


void SendAuthMessages()
{
    // Get the key based off of the ID
    int keyLen;
    char *key = GetKey(id, keyLen);

    // Concatenate the key and the data
    char *full_data = new char[100];
    int index;
    for(index = 0; index < dlc; index++)
        full_data[index] = stmp[index];
    while(*key)
    {
        full_data[index] = key[index - dlc];
        index++;
    } 

    // Create the hash
    unsigned char *hash = MD5::make_hash(full_data);

    // Create the digest
    char *md5str = MD5::make_digest(hash, 20); // 20 bytes
    Serial.println(md5str);
}


void PrintMessage()
{
    Serial.print("ID: ");
    Serial.println(id, HEX);
    Serial.print("DLC: ");
    Serial.println(dlc, HEX);
    Serial.print("Data: ");
    for (int i = 0; i < dlc; i++)
    {
      if (stmp[i] < 0x10)
          Serial.print("0");
      Serial.print(stmp[i], HEX);
      Serial.print(" ");
    }
    Serial.print("\n\r-------------------------------------------\n\r");
}


//---------------------------------------------------------------------------------------
// Generate a random CAN message
//---------------------------------------------------------------------------------------
void GenerateMessage(uint32_t &id, uint8_t &dlc, unsigned char *data)
{
    int selector = random(3);
    id = SelectMessage(selector);
    dlc = random(9);  // dlc = random int between 0..8
    for(int i = 0; i < dlc; i++)
    {
      data[i] = random(0x100); // each byte is random char between 0..0xFF
    }
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
