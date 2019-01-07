// Copyright (c) 2019 Sho Kuroda <krdlab@gmail.com>
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "uid.hpp"
#include <algorithm>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <string>


#define EDDYSTONE_SERVICE_UUID      0xFEAA
#define EDDYSTONE_FRAME_TYPE_UID    0x00

EddystoneUid::EddystoneUid(const std::string& nid, const std::string& bid) {
    frame.len16bitServiceUuidPart = 0x03;   // <- 1 (data type value) + 2 (16-bit Eddystone UUID)
    frame.type16bitServiceUuid = 0x03;      // 16-bit Service UUID
    frame.eddystoneUuid = EDDYSTONE_SERVICE_UUID;
    frame.lenServiceDataPart = 0x17;        // Eddystone-UID full length (23bytes)
    frame.typeServiceData = 0x16;           // Service Data
    frame._eddystoneUuid = EDDYSTONE_SERVICE_UUID;
    frame.frameType = EDDYSTONE_FRAME_TYPE_UID;
    frame.ranging = 0x00;                   // TODO: ranging data
    memset(frame.nid, 0, sizeof(frame.nid));
    memset(frame.bid, 0, sizeof(frame.bid));
    memset(frame.rfu, 0, sizeof(frame.rfu));
    setBeaconId(nid, bid);
}

EddystoneUid::EddystoneUid(const uint8_t* payload) {
    // TODO: validation: check service uuid (0xFEAA) and frame type (0x00)

    const size_t offset = 3; // length (1byte) + Flags data type value (1byte) + Flags data (1byte)
    memcpy(&frame, payload + offset, sizeof(frame));
}

static bool hasEddystoneServiceUuid(const uint8_t* payload) {
    if (payload[3] == 0x03 && payload[4] == 0x03) {
        const uint16_t uuid = (((uint16_t)payload[6]) << 8) + (payload[5]);
        return uuid == EDDYSTONE_SERVICE_UUID;
    }
    return false;
}

static bool isFrameType(const uint8_t* payload, const uint8_t type) {
    if (payload[7] > 3 && payload[8] == 0x16) {
        return payload[11] == type;
    }
    return false;
}

/* static */ bool EddystoneUid::checkPayload(const uint8_t* payload) {
    return hasEddystoneServiceUuid(payload) && isFrameType(payload, EDDYSTONE_FRAME_TYPE_UID);
}

static std::string _id_tolower(const std::string& src) {
    std::string dst = src;
    std::transform(src.begin(), src.end(), dst.begin(), ::tolower);
    return dst;
}

const bool EddystoneUid::has(const std::string& nid, const std::string& bid) const {
    return _id_tolower(nid) == getNamespaceId()
        && _id_tolower(bid) == getInstanceId()
        ;
}

static void _copy(uint8_t* dst, const int* src, const size_t len) {
    for (size_t i = 0; i < len; ++i) {
        dst[i] = (uint8_t) src[i];
    }
}

void EddystoneUid::setBeaconId(const std::string& nid, const std::string& bid) {
    // TODO: validation
    {
        int val[sizeof(frame.nid)] = {0};
        sscanf(
            nid.c_str(),
            "%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x",
            &val[0], &val[1], &val[2], &val[3], &val[4], &val[5], &val[6], &val[7], &val[8], &val[9]);
        _copy(frame.nid, val, sizeof(val) / sizeof(int));
    }
    {
        int val[sizeof(frame.bid)] = {0};
        sscanf(
            bid.c_str(),
            "%2x%2x%2x%2x%2x%2x",
            &val[0], &val[1], &val[2], &val[3], &val[4], &val[5]);
        _copy(frame.bid, val, sizeof(val) / sizeof(int));
    }
}

const std::string EddystoneUid::getNamespaceId() const {
    const uint8_t* p = frame.nid;
    std::stringstream ss;
    ss << std::hex << std::setfill('0')
        << std::setw(2) << (int)p[0]
        << std::setw(2) << (int)p[1]
        << std::setw(2) << (int)p[2]
        << std::setw(2) << (int)p[3]
        << std::setw(2) << (int)p[4]
        << std::setw(2) << (int)p[5]
        << std::setw(2) << (int)p[6]
        << std::setw(2) << (int)p[7]
        << std::setw(2) << (int)p[8]
        << std::setw(2) << (int)p[9]
        ;
    return ss.str();
}

const std::string EddystoneUid::getInstanceId() const {
    const uint8_t* p = frame.bid;
    std::stringstream ss;
    ss << std::hex << std::setfill('0')
        << std::setw(2) << (int)p[0]
        << std::setw(2) << (int)p[1]
        << std::setw(2) << (int)p[2]
        << std::setw(2) << (int)p[3]
        << std::setw(2) << (int)p[4]
        << std::setw(2) << (int)p[5]
        ;
    return ss.str();
}

void EddystoneUid::compose(BLEAdvertisementData& data) const {
    data.setFlags(0x06);    // 0x02 (LE General Discoverable Mode) + 0x04 (BR/EDR Not Supported)
    data.addData(std::string((char*) &frame, sizeof(frame)));
}
