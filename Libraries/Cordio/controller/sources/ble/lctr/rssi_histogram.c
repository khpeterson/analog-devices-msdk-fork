/**
 * @file rssi_histogram.c
 * @brief RSSI histogram tracking for BLE RX packets
 *
 * Tracks RSSI distribution for successful and CRC-failed packets.
 * Designed for minimal overhead - just array indexing and increment.
 *
 * Usage:
 *   1. Call rssi_histogram_init() at boot
 *   2. Call rssi_histogram_record_success(rssi) for each good packet
 *   3. Call rssi_histogram_record_crc_error(rssi) for each CRC failure
 *   4. Call rssi_histogram_report() after each connection TERMINATE
 */

#include <stdint.h>
#include <string.h>
#include "wsf_trace.h"

/* Histogram bins (same as Python analysis) */
#define RSSI_HIST_NUM_BINS 7
#define ERROR_BURST_MAX_BINS 10

/* Bin boundaries: [-100,-80), [-80,-70), [-70,-60), [-60,-50), [-50,-40), [-40,-30), [-30,0] */
static const int8_t rssi_bin_boundaries[RSSI_HIST_NUM_BINS + 1] = {
    -100, -80, -70, -60, -50, -40, -30, 0
};

/* Histogram counters */
static uint32_t rssi_success_hist[RSSI_HIST_NUM_BINS];
static uint32_t rssi_crc_error_hist[RSSI_HIST_NUM_BINS];
static uint32_t rssi_rx_timeout_hist[RSSI_HIST_NUM_BINS];

/* Total packet counts */
static uint32_t total_success;
static uint32_t total_crc_error;
static uint32_t total_rx_timeout;

/* Consecutive error burst histogram (1-10+ consecutive errors) */
static uint32_t error_burst_hist[ERROR_BURST_MAX_BINS];  /* bins: [1], [2], [3], ..., [9], [10+] */
static uint32_t total_error_bursts;

/**
 * @brief Initialize histogram counters
 *
 * Call this at boot time or at connection establishment.
 */
void rssi_histogram_init(void)
{
    memset(rssi_success_hist, 0, sizeof(rssi_success_hist));
    memset(rssi_crc_error_hist, 0, sizeof(rssi_crc_error_hist));
    memset(rssi_rx_timeout_hist, 0, sizeof(rssi_rx_timeout_hist));
    memset(error_burst_hist, 0, sizeof(error_burst_hist));
    total_success = 0;
    total_crc_error = 0;
    total_rx_timeout = 0;
    total_error_bursts = 0;
}

/**
 * @brief Record RSSI for a successful packet
 *
 * @param rssi RSSI value in dBm (e.g., -50 for -50 dBm)
 */
void rssi_histogram_record_success(int8_t rssi)
{
    /* Find bin index */
    int bin = -1;

    for (int i = 0; i < RSSI_HIST_NUM_BINS; i++) {
        if (rssi >= rssi_bin_boundaries[i] && rssi < rssi_bin_boundaries[i + 1]) {
            bin = i;
            break;
        }
    }

    /* Handle out-of-range values (bin them anyway for completeness) */
    if (bin < 0) {
        if (rssi < rssi_bin_boundaries[0]) {
            bin = 0;  /* Below -100 dBm → first bin */
        } else {
            bin = RSSI_HIST_NUM_BINS - 1;  /* Above 0 dBm → last bin */
        }
    }

    /* Increment counter */
    rssi_success_hist[bin]++;
    total_success++;
}

/**
 * @brief Record RSSI for a CRC-failed packet
 *
 * @param rssi RSSI value in dBm (e.g., -65 for -65 dBm)
 */
void rssi_histogram_record_crc_error(int8_t rssi)
{
    /* Find bin index */
    int bin = -1;

    for (int i = 0; i < RSSI_HIST_NUM_BINS; i++) {
        if (rssi >= rssi_bin_boundaries[i] && rssi < rssi_bin_boundaries[i + 1]) {
            bin = i;
            break;
        }
    }

    /* Handle out-of-range values */
    if (bin < 0) {
        if (rssi < rssi_bin_boundaries[0]) {
            bin = 0;  /* Below -100 dBm → first bin */
        } else {
            bin = RSSI_HIST_NUM_BINS - 1;  /* Above 0 dBm → last bin */
        }
    }

    /* Increment counter */
    rssi_crc_error_hist[bin]++;
    total_crc_error++;
}

/**
 * @brief Record RSSI for an RX timeout packet
 *
 * @param rssi RSSI value in dBm (typically 0 for RX_TIMEOUT, but future PHY may provide value)
 */
void rssi_histogram_record_rx_timeout(int8_t rssi)
{
    /* Find bin index */
    int bin = -1;

    for (int i = 0; i < RSSI_HIST_NUM_BINS; i++) {
        if (rssi >= rssi_bin_boundaries[i] && rssi < rssi_bin_boundaries[i + 1]) {
            bin = i;
            break;
        }
    }

    /* Handle out-of-range values */
    if (bin < 0) {
        if (rssi < rssi_bin_boundaries[0]) {
            bin = 0;  /* Below -100 dBm → first bin */
        } else {
            bin = RSSI_HIST_NUM_BINS - 1;  /* Above 0 dBm → last bin */
        }
    }

    /* Increment counter */
    rssi_rx_timeout_hist[bin]++;
    total_rx_timeout++;
}

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
void rssi_histogram_record_error_burst(uint8_t burst_length)
{
    if (burst_length == 0) {
        return;  /* No burst to record */
    }

    /* Bin index: 0 for burst_length=1, 1 for burst_length=2, etc. */
    /* Bins 0-8 are for lengths 1-9, bin 9 is for 10+ */
    uint8_t bin = (burst_length >= ERROR_BURST_MAX_BINS) ? (ERROR_BURST_MAX_BINS - 1) : (burst_length - 1);

    error_burst_hist[bin]++;
    total_error_bursts++;
}

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
 *   RSSI_OK_HIST: 222171,695,14926,20364,56964,109386,19381,432
 *   RSSI_CRC_HIST: 77805,282,36231,33399,2671,3265,1230,726
 *   RSSI_TIMEOUT_HIST: 40525,0,0,0,0,0,0,40525
 *   ERROR_BURST_HIST: 75523,46161,20287,6177,1925,636,222,84,24,6,2
 */
void rssi_histogram_report(void)
{
#if RSSI_HIST_NUM_BINS == 7
    /* Report successful packets - WsfTrace handles up to 8 arguments (total + 7 bins) */
    WsfTrace("RSSI_OK_HIST: %u,%u,%u,%u,%u,%u,%u,%u",
             total_success,
             rssi_success_hist[0],
             rssi_success_hist[1],
             rssi_success_hist[2],
             rssi_success_hist[3],
             rssi_success_hist[4],
             rssi_success_hist[5],
             rssi_success_hist[6]);

    /* Report CRC-failed packets */
    WsfTrace("RSSI_CRC_HIST: %u,%u,%u,%u,%u,%u,%u,%u",
             total_crc_error,
             rssi_crc_error_hist[0],
             rssi_crc_error_hist[1],
             rssi_crc_error_hist[2],
             rssi_crc_error_hist[3],
             rssi_crc_error_hist[4],
             rssi_crc_error_hist[5],
             rssi_crc_error_hist[6]);

    /* Report RX timeout packets (typically all RSSI=0, but future-proof) */
    WsfTrace("RSSI_TIMEOUT_HIST: %u,%u,%u,%u,%u,%u,%u,%u",
             total_rx_timeout,
             rssi_rx_timeout_hist[0],
             rssi_rx_timeout_hist[1],
             rssi_rx_timeout_hist[2],
             rssi_rx_timeout_hist[3],
             rssi_rx_timeout_hist[4],
             rssi_rx_timeout_hist[5],
             rssi_rx_timeout_hist[6]);

    /* Report error burst histogram - total + 10 bins (11 arguments) */
    WsfTrace("ERROR_BURST_HIST: %u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u",
             total_error_bursts,
             error_burst_hist[0],  /* 1 consecutive error */
             error_burst_hist[1],  /* 2 consecutive errors */
             error_burst_hist[2],  /* 3 consecutive errors */
             error_burst_hist[3],  /* 4 consecutive errors */
             error_burst_hist[4],  /* 5 consecutive errors */
             error_burst_hist[5],  /* 6 consecutive errors */
             error_burst_hist[6],  /* 7 consecutive errors */
             error_burst_hist[7],  /* 8 consecutive errors */
             error_burst_hist[8],  /* 9 consecutive errors */
             error_burst_hist[9]); /* 10+ consecutive errors */
#else
#error "RSSI_HIST_NUM_BINS must be 7 for WsfTrace compatibility"
#endif
}

/**
 * @brief Report histogram with human-readable labels (for debug)
 *
 * Use this for interactive debugging. For automated collection, use rssi_histogram_report().
 * Note: This uses more trace bandwidth - only use for debug, not production.
 */
void rssi_histogram_report_verbose(void)
{
#if RSSI_HIST_NUM_BINS == 7
    WsfTrace("=== RSSI HISTOGRAM ===");
    WsfTrace("SUCCESS (n=%u):", total_success);

    for (int i = 0; i < RSSI_HIST_NUM_BINS; i++) {
        if (total_success > 0) {
            uint32_t pct_times_10 = (1000 * rssi_success_hist[i]) / total_success;
            WsfTrace("  %d to %d dBm: %u (%u.%u%%)",
                     rssi_bin_boundaries[i],
                     rssi_bin_boundaries[i + 1],
                     rssi_success_hist[i],
                     pct_times_10 / 10,
                     pct_times_10 % 10);
        } else {
            WsfTrace("  %d to %d dBm: %u",
                     rssi_bin_boundaries[i],
                     rssi_bin_boundaries[i + 1],
                     rssi_success_hist[i]);
        }
    }

    WsfTrace("CRC_FAILED (n=%u):", total_crc_error);

    for (int i = 0; i < RSSI_HIST_NUM_BINS; i++) {
        if (total_crc_error > 0) {
            uint32_t pct_times_10 = (1000 * rssi_crc_error_hist[i]) / total_crc_error;
            WsfTrace("  %d to %d dBm: %u (%u.%u%%)",
                     rssi_bin_boundaries[i],
                     rssi_bin_boundaries[i + 1],
                     rssi_crc_error_hist[i],
                     pct_times_10 / 10,
                     pct_times_10 % 10);
        } else {
            WsfTrace("  %d to %d dBm: %u",
                     rssi_bin_boundaries[i],
                     rssi_bin_boundaries[i + 1],
                     rssi_crc_error_hist[i]);
        }
    }

    WsfTrace("=====================");
#else
#error "RSSI_HIST_NUM_BINS must be 7 for WsfTrace compatibility"
#endif
}

/**
 * @brief Get bin index for a given RSSI value (utility function)
 *
 * @param rssi RSSI value in dBm
 * @return Bin index (0 to RSSI_HIST_NUM_BINS-1), or -1 if out of range
 */
int rssi_histogram_get_bin(int8_t rssi)
{
    for (int i = 0; i < RSSI_HIST_NUM_BINS; i++) {
        if (rssi >= rssi_bin_boundaries[i] && rssi < rssi_bin_boundaries[i + 1]) {
            return i;
        }
    }
    return -1;
}
