#include "ModbusSlave.h"

#define SLAVE_ID 1
#define CTRL_PIN 0
#define BAUDRATE 115200

Modbus slave(SLAVE_ID, CTRL_PIN);

void setup()
{
  analogReference(INTERNAL);

  for (int i = 2; i < 8; i++)
    pinMode(i, INPUT_PULLUP);

  for (int i = 8; i < 14; i++)
    pinMode(i, OUTPUT); 
  
  slave.cbVector[CB_READ_DISCRETE_INPUTS] = readDigitalIn;
  slave.cbVector[CB_READ_INPUT_REGISTERS] = readAnalogIn;
  slave.cbVector[CB_WRITE_COILS] = writeDigitalOut;
  
  Serial.begin(BAUDRATE, SERIAL_8E1);
  slave.begin(BAUDRATE);
}

void loop()
{
  slave.poll();
}

uint8_t readDigitalIn(uint8_t fc, uint16_t offset, uint16_t length)
{
  if (offset + length <= 6)
    for (int i = 0; i < length; i++)
      slave.writeCoilToBuffer(i, digitalRead(offset + i + 2));
  else return STATUS_ILLEGAL_DATA_ADDRESS;
        
  return STATUS_OK;
}

uint8_t readAnalogIn(uint8_t fc, uint16_t offset, uint16_t length)
{
  if (offset + length <= 6)
    for (int i = 0; i < length; i++)
      slave.writeRegisterToBuffer(i, analogRead(offset + i));
  else return STATUS_ILLEGAL_DATA_ADDRESS;
  
  return STATUS_OK;
}

uint8_t writeDigitalOut(uint8_t fc, uint16_t offset, uint16_t length)
{
  if (offset + length <= 6)
    for (int i = 0; i < length; i++)
      digitalWrite(offset + i + 8, slave.readCoilFromBuffer(i));
  else return STATUS_ILLEGAL_DATA_ADDRESS;
  
  return STATUS_OK;
}
