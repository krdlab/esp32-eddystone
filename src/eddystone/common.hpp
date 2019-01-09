// Copyright (c) 2019 Sho Kuroda <krdlab@gmail.com>
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef EDDYSTONE_COMMON_HPP
#define EDDYSTONE_COMMON_HPP

#include <cstdint>

#define EDDYSTONE_SERVICE_UUID      0xFEAA
#define EDDYSTONE_FRAME_TYPE_UID    0x00

namespace eddystone {
    inline static bool hasEddystoneServiceUuid(const uint8_t* payload) {
        if (payload[3] == 0x03 && payload[4] == 0x03) {
            const uint16_t uuid = (((uint16_t)payload[6]) << 8) + (payload[5]);
            return uuid == EDDYSTONE_SERVICE_UUID;
        }
        return false;
    }

    inline static bool isFrameType(const uint8_t* payload, const uint8_t type) {
        if (payload[7] > 3 && payload[8] == 0x16) {
            return payload[11] == type;
        }
        return false;
    }
}

#endif
