// Copyright (c) 2019 Sho Kuroda <krdlab@gmail.com>
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include <BLEDevice.h>
#include <BLEServer.h>
#include <Arduino.h>
#include <eddystone.hpp>


const std::string NAMESPACE = "AABBCCDDEEFF11223344";
const std::string INSTANCE  = "112233445566";

void setBeacon(BLEAdvertising* pAdvertising) {
  EddystoneUid uid(NAMESPACE, INSTANCE);
  BLEAdvertisementData adData;
  uid.compose(adData);
  pAdvertising->setAdvertisementData(adData);
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }
  
  BLEDevice::init("");
  BLEServer *pServer = BLEDevice::createServer();
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  setBeacon(pAdvertising);

  Serial.println("Advertising for 1 seccond only...");
  pAdvertising->start();
  delay(1000);
  pAdvertising->stop();
  Serial.println("Advertising stopped.");
}

void loop() {
  delay(1000);
}
