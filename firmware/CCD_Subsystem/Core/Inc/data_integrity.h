/*
 * data_integrity.h
 *
 *  Created on: Sep 22, 2025
 *      Author: Shrek
 */

#ifndef INC_DATA_INTEGRITY_H_
#define INC_DATA_INTEGRITY_H_

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    ERROR_NONE,
    ERROR_CAPTURE,
    ERROR_BUFFER,
    ERROR_FEEDBACK
} ErrorState;

typedef struct {
    uint16_t checksum;
    ErrorState error_state;
} DataIntegrity;

uint16_t calc_checksum(const uint8_t *data, uint32_t length);
bool verify_checksum(const uint8_t *data, uint32_t length, uint16_t expected);

void error_capturing_image(DataIntegrity *di);
void error_accessing_buffer(DataIntegrity *di);
void error_receiving_feedback(DataIntegrity *di);


#endif /* INC_DATA_INTEGRITY_H_ */
