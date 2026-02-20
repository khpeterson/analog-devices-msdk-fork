/*************************************************************************************************/
/*!
 *  \file
 *
 *  \brief  Rx histogram tracking: RSSI distributions and consecutive error bursts.
 *
 *  Tracks RSSI distributions for three packet outcomes (success, CRC error, RX timeout) and a
 *  histogram of consecutive Rx error burst lengths.  Designed for minimal overhead - just array
 *  indexing and increment.
 *
 *  Usage:
 *    1. Call lctrHistInit() at boot or at connection establishment
 *    2. Call lctrHistRecordSuccess(rssi) for each successfully received packet
 *    3. Call lctrHistRecordCrcError(rssi) for each CRC failure
 *    4. Call lctrHistRecordRxTimeout(rssi) for each RX timeout
 *    5. Call lctrHistRecordErrorBurst(n) when an error run ends (n > 0)
 *    6. Call lctrHistReport() after each connection TERMINATE
 */
/*************************************************************************************************/

#include <stdint.h>
#include <string.h>
#include "wsf_trace.h"

/* Histogram bins (same as Python analysis) */
#define RSSI_HIST_NUM_BINS   7
#define ERROR_BURST_MAX_BINS 15

/* Bin boundaries: [-100,-80), [-80,-70), [-70,-60), [-60,-50), [-50,-40), [-40,-30), [-30,0] */
static const int8_t rssiBinBoundaries[RSSI_HIST_NUM_BINS + 1] = {
  -100, -80, -70, -60, -50, -40, -30, 0
};

/* Histogram counters */
static uint32_t rssiSuccessHist[RSSI_HIST_NUM_BINS];
static uint32_t rssiCrcErrorHist[RSSI_HIST_NUM_BINS];
static uint32_t rssiRxTimeoutHist[RSSI_HIST_NUM_BINS];

/* Total packet counts */
static uint32_t totalSuccess;
static uint32_t totalCrcError;
static uint32_t totalRxTimeout;

/* Consecutive error burst histogram (1-15+ consecutive errors) */
static uint32_t errorBurstHist[ERROR_BURST_MAX_BINS];  /* bins: [1], [2], [3], ..., [14], [15+] */
static uint32_t totalErrorBursts;

/* Map an RSSI value to a bin index, clamping out-of-range values to the nearest bin. */
static int lctrHistGetRssiBin(int8_t rssi)
{
  for (int i = 0; i < RSSI_HIST_NUM_BINS; i++)
  {
    if (rssi >= rssiBinBoundaries[i] && rssi < rssiBinBoundaries[i + 1])
    {
      return i;
    }
  }
  return (rssi < rssiBinBoundaries[0]) ? 0 : (RSSI_HIST_NUM_BINS - 1);
}

/*************************************************************************************************/
/*!
 *  \brief  Initialize histogram counters.
 *
 *  Call this at boot time or at connection establishment.
 */
/*************************************************************************************************/
void lctrHistInit(void)
{
  memset(rssiSuccessHist,  0, sizeof(rssiSuccessHist));
  memset(rssiCrcErrorHist, 0, sizeof(rssiCrcErrorHist));
  memset(rssiRxTimeoutHist, 0, sizeof(rssiRxTimeoutHist));
  memset(errorBurstHist,   0, sizeof(errorBurstHist));
  totalSuccess     = 0;
  totalCrcError    = 0;
  totalRxTimeout   = 0;
  totalErrorBursts = 0;
}

/*************************************************************************************************/
/*!
 *  \brief  Record RSSI for a successfully received packet.
 *
 *  \param  rssi    RSSI value in dBm (e.g., -50 for -50 dBm).
 */
/*************************************************************************************************/
void lctrHistRecordSuccess(int8_t rssi)
{
  int bin = lctrHistGetRssiBin(rssi);
  rssiSuccessHist[bin]++;
  totalSuccess++;
}

/*************************************************************************************************/
/*!
 *  \brief  Record RSSI for a CRC-failed packet.
 *
 *  \param  rssi    RSSI value in dBm (e.g., -65 for -65 dBm).
 */
/*************************************************************************************************/
void lctrHistRecordCrcError(int8_t rssi)
{
  int bin = lctrHistGetRssiBin(rssi);
  rssiCrcErrorHist[bin]++;
  totalCrcError++;
}

/*************************************************************************************************/
/*!
 *  \brief  Record RSSI for an RX timeout.
 *
 *  \param  rssi    RSSI value in dBm (typically 0 for RX_TIMEOUT, but future PHY may provide value).
 */
/*************************************************************************************************/
void lctrHistRecordRxTimeout(int8_t rssi)
{
  int bin = lctrHistGetRssiBin(rssi);
  rssiRxTimeoutHist[bin]++;
  totalRxTimeout++;
}

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
void lctrHistRecordErrorBurst(uint8_t burstLength)
{
  if (burstLength == 0)
  {
    return;  /* No burst to record */
  }

  /* Bin index: 0 for burstLength=1, 1 for burstLength=2, etc. */
  /* Bins 0-13 are for lengths 1-14, bin 14 is for 15+ */
  int bin = (burstLength >= ERROR_BURST_MAX_BINS) ? (ERROR_BURST_MAX_BINS - 1) : (burstLength - 1);
  errorBurstHist[bin]++;
  totalErrorBursts++;
}

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
 *    RSSI_OK_HIST: 222171,695,14926,20364,56964,109386,19381,432
 *    RSSI_CRC_HIST: 77805,282,36231,33399,2671,3265,1230,726
 *    RSSI_TIMEOUT_HIST: 40525,0,0,0,0,0,0,40525
 *    ERROR_BURST_HIST: 75523,46161,20287,6177,1925,636,222,84,24,6,2,1,1,0,0,0
 */
/*************************************************************************************************/
void lctrHistReport(void)
{
#if RSSI_HIST_NUM_BINS == 7
  WsfTrace("RSSI_OK_HIST: %u,%u,%u,%u,%u,%u,%u,%u",
           totalSuccess,
           rssiSuccessHist[0],
           rssiSuccessHist[1],
           rssiSuccessHist[2],
           rssiSuccessHist[3],
           rssiSuccessHist[4],
           rssiSuccessHist[5],
           rssiSuccessHist[6]);

  /* Report CRC-failed packets */
  WsfTrace("RSSI_CRC_HIST: %u,%u,%u,%u,%u,%u,%u,%u",
           totalCrcError,
           rssiCrcErrorHist[0],
           rssiCrcErrorHist[1],
           rssiCrcErrorHist[2],
           rssiCrcErrorHist[3],
           rssiCrcErrorHist[4],
           rssiCrcErrorHist[5],
           rssiCrcErrorHist[6]);

  /* Report RX timeout packets (typically all RSSI=0, but future-proof) */
  WsfTrace("RSSI_TIMEOUT_HIST: %u,%u,%u,%u,%u,%u,%u,%u",
           totalRxTimeout,
           rssiRxTimeoutHist[0],
           rssiRxTimeoutHist[1],
           rssiRxTimeoutHist[2],
           rssiRxTimeoutHist[3],
           rssiRxTimeoutHist[4],
           rssiRxTimeoutHist[5],
           rssiRxTimeoutHist[6]);
#else
#error "RSSI_HIST_NUM_BINS must be 7"
#endif

#if ERROR_BURST_MAX_BINS == 15
  /* Report error burst histogram - total + 15 bins (16 arguments) */
  WsfTrace("ERROR_BURST_HIST: %u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u",
           totalErrorBursts,
           errorBurstHist[0],   /* 1 consecutive error */
           errorBurstHist[1],   /* 2 consecutive errors */
           errorBurstHist[2],   /* 3 consecutive errors */
           errorBurstHist[3],   /* 4 consecutive errors */
           errorBurstHist[4],   /* 5 consecutive errors */
           errorBurstHist[5],   /* 6 consecutive errors */
           errorBurstHist[6],   /* 7 consecutive errors */
           errorBurstHist[7],   /* 8 consecutive errors */
           errorBurstHist[8],   /* 9 consecutive errors */
           errorBurstHist[9],   /* 10 consecutive errors */
           errorBurstHist[10],  /* 11 consecutive errors */
           errorBurstHist[11],  /* 12 consecutive errors */
           errorBurstHist[12],  /* 13 consecutive errors */
           errorBurstHist[13],  /* 14 consecutive errors */
           errorBurstHist[14]); /* 15+ consecutive errors */
#else
#error "ERROR_BURST_MAX_BINS must be 15"
#endif
}

/*************************************************************************************************/
/*!
 *  \brief  Report all histograms with human-readable labels (for debug).
 *
 *  Prints all four histograms (RSSI success, RSSI CRC error, RSSI RX timeout, and consecutive
 *  error bursts) with per-bin percentages.  For automated post-processing, use lctrHistReport()
 *  instead.  This uses more trace bandwidth; only use for debug, not production.
 */
/*************************************************************************************************/
void lctrHistReportVerbose(void)
{
  WsfTrace("=== RX HISTOGRAMS ===");
  WsfTrace("SUCCESS (n=%u):", totalSuccess);

  for (int i = 0; i < RSSI_HIST_NUM_BINS; i++)
  {
    if (totalSuccess > 0)
    {
      uint32_t pctTimes10 = (1000 * rssiSuccessHist[i]) / totalSuccess;
      WsfTrace("  %d to %d dBm: %u (%u.%u%%)",
               rssiBinBoundaries[i],
               rssiBinBoundaries[i + 1],
               rssiSuccessHist[i],
               pctTimes10 / 10,
               pctTimes10 % 10);
    }
    else
    {
      WsfTrace("  %d to %d dBm: %u",
               rssiBinBoundaries[i],
               rssiBinBoundaries[i + 1],
               rssiSuccessHist[i]);
    }
  }

  WsfTrace("CRC_FAILED (n=%u):", totalCrcError);

  for (int i = 0; i < RSSI_HIST_NUM_BINS; i++)
  {
    if (totalCrcError > 0)
    {
      uint32_t pctTimes10 = (1000 * rssiCrcErrorHist[i]) / totalCrcError;
      WsfTrace("  %d to %d dBm: %u (%u.%u%%)",
               rssiBinBoundaries[i],
               rssiBinBoundaries[i + 1],
               rssiCrcErrorHist[i],
               pctTimes10 / 10,
               pctTimes10 % 10);
    }
    else
    {
      WsfTrace("  %d to %d dBm: %u",
               rssiBinBoundaries[i],
               rssiBinBoundaries[i + 1],
               rssiCrcErrorHist[i]);
    }
  }

  WsfTrace("RX_TIMEOUT (n=%u):", totalRxTimeout);

  for (int i = 0; i < RSSI_HIST_NUM_BINS; i++)
  {
    if (totalRxTimeout > 0)
    {
      uint32_t pctTimes10 = (1000 * rssiRxTimeoutHist[i]) / totalRxTimeout;
      WsfTrace("  %d to %d dBm: %u (%u.%u%%)",
               rssiBinBoundaries[i],
               rssiBinBoundaries[i + 1],
               rssiRxTimeoutHist[i],
               pctTimes10 / 10,
               pctTimes10 % 10);
    }
    else
    {
      WsfTrace("  %d to %d dBm: %u",
               rssiBinBoundaries[i],
               rssiBinBoundaries[i + 1],
               rssiRxTimeoutHist[i]);
    }
  }

  WsfTrace("ERROR_BURSTS (n=%u):", totalErrorBursts);

  for (int i = 0; i < ERROR_BURST_MAX_BINS; i++)
  {
    int burstLen = i + 1;
    if (totalErrorBursts > 0)
    {
      uint32_t pctTimes10 = (1000 * errorBurstHist[i]) / totalErrorBursts;
      if (burstLen < ERROR_BURST_MAX_BINS)
      {
        WsfTrace("  len %d: %u (%u.%u%%)",
                 burstLen, errorBurstHist[i],
                 pctTimes10 / 10, pctTimes10 % 10);
      }
      else
      {
        WsfTrace("  len %d+: %u (%u.%u%%)",
                 burstLen, errorBurstHist[i],
                 pctTimes10 / 10, pctTimes10 % 10);
      }
    }
    else
    {
      if (burstLen < ERROR_BURST_MAX_BINS)
      {
        WsfTrace("  len %d: %u", burstLen, errorBurstHist[i]);
      }
      else
      {
        WsfTrace("  len %d+: %u", burstLen, errorBurstHist[i]);
      }
    }
  }

  WsfTrace("====================");
}
