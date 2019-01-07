// Copyright (c) 2019 Sho Kuroda <krdlab@gmail.com>
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include <BLEAdvertisedDevice.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <Arduino.h>
#include <eddystone.hpp>


const int SCAN_TIME = 30; // scanning time (sec)

class AdvertisedDeviceHandler : public BLEAdvertisedDeviceCallbacks {
private:
  const std::string NAMESPACE = "AABBCCDDEEFF11223344";
  const std::string INSTANCE  = "112233445566";
public:
  void onResult(BLEAdvertisedDevice device) {
    bool found = false;
    if (EddystoneUid::checkPayload(device.getPayload())) {
      EddystoneUid uid(device.getPayload());
      if (uid.has(NAMESPACE, INSTANCE)) {
        Serial.printf("> Target beacon found.\n");
        Serial.printf("  Address: %s\n", device.getAddress().toString().c_str());
        found = true;
      }
    }
    if (!found) {
      Serial.printf("> %s\n", device.toString().c_str());
    }
  }
};

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  BLEDevice::init("");
  BLEScan* scanner = BLEDevice::getScan(); // singleton object
  scanner->setAdvertisedDeviceCallbacks(new AdvertisedDeviceHandler());

  Serial.printf("> Start scanning...(%d sec)\n", SCAN_TIME);
  BLEScanResults devices = scanner->start(SCAN_TIME);
  Serial.printf("> Devices found: %d\n", devices.getCount());
}

void loop() {
  delay(1000);
}
