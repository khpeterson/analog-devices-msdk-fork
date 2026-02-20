/*************************************************************************************************/
/*!
 *  \file
 *
 *  \brief  Rx histogram tracking: RSSI distributions and consecutive error bursts.
 */
/*************************************************************************************************/

#ifndef LCTR_HIST_H
#define LCTR_HIST_H

#include <stdint.h>

/*************************************************************************************************/
/*!
 *  \brief  Initialize histogram counters.
 *
 *  Call this at boot time or at connection establishment.
 */
/*************************************************************************************************/
void lctrHistInit(void);

/*************************************************************************************************/
/*!
 *  \brief  Record RSSI for a successfully received packet.
 *
 *  \param  rssi    RSSI value in dBm (e.g., -50 for -50 dBm).
 */
/*************************************************************************************************/
void lctrHistRecordSuccess(int8_t rssi);

/*************************************************************************************************/
/*!
 *  \brief  Record RSSI for a CRC-failed packet.
 *
 *  \param  rssi    RSSI value in dBm (e.g., -65 for -65 dBm).
 */
/*************************************************************************************************/
void lctrHistRecordCrcError(int8_t rssi);

/*************************************************************************************************/
/*!
 *  \brief  Record RSSI for an RX timeout.
 *
 *  \param  rssi    RSSI value in dBm (typically 0 for RX_TIMEOUT, but future PHY may provide value).
 */
/*************************************************************************************************/
void lctrHistRecordRxTimeout(int8_t rssi);

/*************************************************************************************************/
/*!
 *  \brief  Record consecutive error burst length.
 *
 *  Call this when an error burst ends (i.e., when a SUCCESS follows errors).
 *
 *  \param  burstLength   Number of consecutive errors (1-15+).
 *                        1   = single isolated error
 *                        2   = two consecutive errors
 *                        ...
 *                        15+ = fifteen or more consecutive errors (binned together)
 */
/*************************************************************************************************/
void lctrHistRecordErrorBurst(uint8_t burstLength);

/*************************************************************************************************/
/*!
 *  \brief  Report histogram data (minimal format for post-processing).
 *
 *  Call this after each connection TERMINATE event.
 *  Output format (four lines):
 *    RSSI_OK_HIST:      total,bin0,bin1,...,bin6
 *    RSSI_CRC_HIST:     total,bin0,bin1,...,bin6
 *    RSSI_TIMEOUT_HIST: total,bin0,bin1,...,bin6
 *    ERROR_BURST_HIST:  total,bin1,bin2,...,bin15plus
 *
 *  Example output:
 *    RSSI_OK_HIST: 184613,403,2313,11923,36950,66209,66751,64
 *    RSSI_CRC_HIST: 51715,1796,6247,12163,5873,6975,18572,89
 *    RSSI_TIMEOUT_HIST: 35286,0,0,0,0,0,0,35286
 *    ERROR_BURST_HIST: 58584,39634,12591,4253,1449,435,146,49,16,6,5,2,1,0,0,0
 */
/*************************************************************************************************/
void lctrHistReport(void);

/*************************************************************************************************/
/*!
 *  \brief  Report all histograms with human-readable labels (for debug).
 *
 *  Prints all four histograms (RSSI success, RSSI CRC error, RSSI RX timeout, and consecutive
 *  error bursts) with per-bin percentages.  For automated post-processing, use lctrHistReport()
 *  instead.  This uses more trace bandwidth; only use for debug, not production.
 */
/*************************************************************************************************/
void lctrHistReportVerbose(void);

#endif /* LCTR_HIST_H */
