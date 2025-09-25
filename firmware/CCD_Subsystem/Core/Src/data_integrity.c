/*
 * data_integrity.c
 *
 *  Created on: Sep 22, 2025
 *      Author: Shrek
 */

#include "data_integrity.h"

uint16_t calc_checksum(const uint8_t *data, uint32_t length) {
    uint16_t sum = 0;
    for (uint32_t i = 0; i < length; i++) {
        sum += data[i];
    }
    return sum;
}

bool verify_checksum(const uint8_t *data, uint32_t length, uint16_t expected) {
    return calc_checksum(data, length) == expected;
}

void error_capturing_image(DataIntegrity *di) {
    di->error_state = ERROR_CAPTURE;
}

void error_accessing_buffer(DataIntegrity *di) {
    di->error_state = ERROR_BUFFER;
}

void error_receiving_feedback(DataIntegrity *di) {
    di->error_state = ERROR_FEEDBACK;
}

