// Copyright (c) 2019 Sho Kuroda <krdlab@gmail.com>
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef EDDYSTONE_UID_HPP
#define EDDYSTONE_UID_HPP

#include <BLEAdvertising.h>
#include <string>

class EddystoneUid {
private:
    struct {
        uint8_t  len16bitServiceUuidPart;
        uint8_t  type16bitServiceUuid;
        uint16_t eddystoneUuid;
        uint8_t  lenServiceDataPart;
        uint8_t  typeServiceData;
        uint16_t _eddystoneUuid;
        uint8_t  frameType;
        uint8_t  ranging;
        uint8_t  nid[10];
        uint8_t  bid[6];
        uint8_t  rfu[2];
    } __attribute__((packed)) frame;

    void setBeaconId(const std::string& nid, const std::string& bid);
public:
    EddystoneUid(const std::string& nid, const std::string& bid);
    explicit EddystoneUid(const uint8_t* payload);

    static bool checkPayload(const uint8_t* payload);

    const bool has(const std::string& nid, const std::string& bid) const;
    const std::string getNamespaceId() const;
    const std::string getInstanceId() const;

    void compose(BLEAdvertisementData& data) const;
};

#endif
