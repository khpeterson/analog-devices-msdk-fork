/**
 * @file rssi_histogram.h
 * @brief RSSI histogram tracking for BLE RX packets
 */

#ifndef RSSI_HISTOGRAM_H
#define RSSI_HISTOGRAM_H

#include <stdint.h>

/**
 * @brief Initialize histogram counters
 *
 * Call this at boot time or at connection establishment.
 */
void rssi_histogram_init(void);

/**
 * @brief Record RSSI for a successful packet
 *
 * @param rssi RSSI value in dBm (e.g., -50 for -50 dBm)
 */
void rssi_histogram_record_success(int8_t rssi);

/**
 * @brief Record RSSI for a CRC-failed packet
 *
 * @param rssi RSSI value in dBm (e.g., -65 for -65 dBm)
 */
void rssi_histogram_record_crc_error(int8_t rssi);

/**
 * @brief Record RSSI for an RX timeout packet
 *
 * @param rssi RSSI value in dBm (typically 0 for RX_TIMEOUT, but future PHY may provide value)
 */
void rssi_histogram_record_rx_timeout(int8_t rssi);

/**
 * @brief Record consecutive error burst length
 *
 * Call this when an error burst ends (i.e., when a SUCCESS follows errors).
 *
 * @param burst_length Number of consecutive errors (1-10+)
 *                     1 = single isolated error
 *                     2 = two consecutive errors
 *                     ...
 *                     10+ = ten or more consecutive errors (binned together)
 */
void rssi_histogram_record_error_burst(uint8_t burst_length);

/**
 * @brief Report histogram data (minimal format for post-processing)
 *
 * Call this after each connection TERMINATE event.
 * Output format (four lines):
 *   RSSI_OK_HIST: total,bin0,bin1,...,bin6
 *   RSSI_CRC_HIST: total,bin0,bin1,...,bin6
 *   RSSI_TIMEOUT_HIST: total,bin0,bin1,...,bin6
 *   ERROR_BURST_HIST: total,bin1,bin2,...,bin10plus
 *
 * Example output:
 *   RSSI_OK_HIST: 184613,403,2313,11923,36950,66209,66751,64
 *   RSSI_CRC_HIST: 51715,1796,6247,12163,5873,6975,18572,89
 *   RSSI_TIMEOUT_HIST: 35286,0,0,0,0,0,0,35286
 *   ERROR_BURST_HIST: 58584,39634,12591,4253,1449,435,146,49,16,6,5
 */
void rssi_histogram_report(void);

/**
 * @brief Report histogram with human-readable labels (for debug)
 *
 * Use this for interactive debugging. For automated collection, use rssi_histogram_report().
 */
void rssi_histogram_report_verbose(void);

/**
 * @brief Get bin index for a given RSSI value (utility function)
 *
 * @param rssi RSSI value in dBm
 * @return Bin index (0 to 6), or -1 if out of range
 */
int rssi_histogram_get_bin(int8_t rssi);

#endif /* RSSI_HISTOGRAM_H */
