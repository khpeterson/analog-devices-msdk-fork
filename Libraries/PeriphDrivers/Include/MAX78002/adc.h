/**
 * @file    adc.h
 * @brief   Analog to Digital Converter(ADC) function prototypes and data types.
 */

/* ****************************************************************************
 * Copyright (C) 2019 Maxim Integrated Products, Inc., All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of Maxim Integrated
 * Products, Inc. shall not be used except as stated in the Maxim Integrated
 * Products, Inc. Branding Policy.
 *
 * The mere transfer of this software does not imply any licenses
 * of trade secrets, proprietary technology, copyrights, patents,
 * trademarks, maskwork rights, or any other form of intellectual
 * property whatsoever. Maxim Integrated Products, Inc. retains all
 * ownership rights.
 *
 * $Date: 2018-08-28 17:03:02 -0500 (Tue, 28 Aug 2018) $
 * $Revision: 37424 $
 *
 *************************************************************************** */

/* Define to prevent redundant inclusion */
#ifndef LIBRARIES_PERIPHDRIVERS_INCLUDE_MAX78002_ADC_H_
#define LIBRARIES_PERIPHDRIVERS_INCLUDE_MAX78002_ADC_H_

/* **** Includes **** */
#include <stdint.h>
#include "adc_regs.h"
#include "mcr_regs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup adc ADC
 * @ingroup periphlibs
 * @{
 */

/* MAX78002 Specific TODO */
#define MAX_ADC_SLOT_NUM 20
#define MAX_ADC_FIFO_LEN 16
#define MAX_ADC_RES_DIV_CH 8

/***************************************************************************************************************
                                    DATA STRUCTURES FOR ADC INITIALIZATION
***************************************************************************************************************/

/**
  * @brief  Enumeration type for the ADC Input Channels
  *
  */
typedef enum {
    MXC_ADC_CH_0, ///< Select Channel 0
    MXC_ADC_CH_1, ///< Select Channel 1
    MXC_ADC_CH_2, ///< Select Channel 2
    MXC_ADC_CH_3, ///< Select Channel 3
    MXC_ADC_CH_4, ///< Select Channel 4
    MXC_ADC_CH_5, ///< Select Channel 5
    MXC_ADC_CH_6, ///< Select Channel 6
    MXC_ADC_CH_7, ///< Select Channel 7
    MXC_ADC_CH_VCOREA, ///< Select Channel 8,  VCOREA
    MXC_ADC_CH_VCOREB, ///< Select Channel 9,  VCOREB
    MXC_ADC_CH_VLDO_2P5_DIV4, ///< Select Channel 10, VLDO_2P5DIV4
    MXC_ADC_CH_VLDO_0P9, ///< Select Channel 11, VLDO_0P9
    MXC_ADC_CH_VDDA_DIV2, ///< Select Channel 12, VDDA_DIV2
    MXC_ADC_CH_TEMP_SENSOR, ///< Select Channel 13, VDDA_DIV2
    MXC_ADC_CH_VBB_DIV4, ///< Select Channel 14, VBB_DIV4
    MXC_ADC_CH_VDDB_DIV4, ///< Select Channel 14, VDDB_DIV4
    MXC_ADC_CH_VSS, ///< Select Channel 16, VSS
} mxc_adc_chsel_t;

// AI87-TODO: Github comment the below enum out - ?
/**
  * @brief  Enumeration type for the number of samples to average
  *
  */
typedef enum {
    MXC_ADC_AVG_1 = MXC_S_ADC_CTRL1_AVG_AVG1, ///< Select Channel 0
    MXC_ADC_AVG_2 = MXC_S_ADC_CTRL1_AVG_AVG2, ///< Select Channel 1
    MXC_ADC_AVG_4 = MXC_S_ADC_CTRL1_AVG_AVG4, ///< Select Channel 2
    MXC_ADC_AVG_8 = MXC_S_ADC_CTRL1_AVG_AVG8, ///< Select Channel 3
    MXC_ADC_AVG_16 = MXC_S_ADC_CTRL1_AVG_AVG16, ///< Select Channel 4
    MXC_ADC_AVG_32 = MXC_S_ADC_CTRL1_AVG_AVG32, ///< Select Channel 5
} mxc_adc_avg_t;

/**
  * Enumeration type for the ADC Compartors
  */
typedef enum {
    MXC_ADC_COMP_0 = 1,
    MXC_ADC_COMP_1 = 2,
    // MXC_ADC_COMP_HYST_0  = 1 << MXC_F_MCR_AINCOMP_AINCOMPHYST_POS,    //Refer to the V HYST specification in the data sheet electrical characteristics
    // // for the values corresponding to this field value.
    // MXC_ADC_COMP_HYST_1     = 2 << MXC_F_MCR_AINCOMP_AINCOMPHYST_POS,   //Refer to the V HYST specification in the data sheet electrical characteristics
    // for the values corresponding to this field value.
} mxc_adc_comp_t;

/**
 * @brief       Enumeration type for ADC clock divider
 */
typedef enum {
    MXC_ADC_CLKDIV_2, ///< ADC Scale by 1/2
    MXC_ADC_CLKDIV_4, ///< ADC Scale by 1/4
    MXC_ADC_CLKDIV_8, ///< ADC Scale by 1/8
    MXC_ADC_CLKDIV_16, ///< ADC Scale by 1/16
    MXC_ADC_CLKDIV_1, ///< ADC Scale by 1x (no scaling)
} mxc_adc_clkdiv_t;

/**
 * @brief       Clock settings
 */
typedef enum {
    MXC_ADC_CLK_HCLK, ///< HCLK CLock
    MXC_ADC_CLK_EXT, ///< External Clock
    MXC_ADC_CLK_IBRO, ///< IBRO Clock
    MXC_ADC_CLK_ERFO, ///< ERFO Clock
} mxc_adc_clock_t;

/**
 * @brief       Calibration settings 
 */
typedef enum {
    MXC_ADC_SKIP_CAL, ///< Skip calibration on power-up
    MXC_ADC_EN_CAL, ///< Calibrate on power-up
} mxc_adc_calibration_t;

/**
 * @brief       trigger mode settings 
 */
typedef enum {
    MXC_ADC_TRIG_SOFTWARE, ///< Software Trigger
    MXC_ADC_TRIG_HARDWARE, ///< Hardware Trigger
} mxc_adc_trig_mode_t;

/**
 * @brief       Hardware trigger select options
 */
typedef enum {
    MXC_ADC_TRIG_SEL_TMR0, ///< Timer 0 Out Rising edge
    MXC_ADC_TRIG_SEL_TMR1, ///< Timer 1 Out Rising Edge
    MXC_ADC_TRIG_SEL_TMR2, ///< Timer 2 Out Rising Edge
    MXC_ADC_TRIG_SEL_TMR3, ///< Timer 3 Out Rising Edge
    MXC_ADC_TRIG_SEL_P1_12, ///< GPIO Port 1, Pin 12
    MXC_ADC_TRIG_SEL_P1_13, ///< GPIO Port 1, Pin 13
    MXC_ADC_TRIG_SEL_P1_14, ///< GPIO Port 1, Pin 14
    MXC_ADC_TRIG_SEL_TEMP_SENS, ///< Temperature Sensor Ready
} mxc_adc_trig_sel_t;

/**
 * @brief       trigger mode settings 
 */
typedef enum {
    MXC_ADC_ATOMIC_CONV, ///< Software Trigger
    MXC_ADC_CONTINUOUS_CONV, ///< Hardware Trigger
} mxc_adc_conversion_mode_t;

/**
 * @brief  Analog input divider select.
 */
typedef enum {
    MXC_ADC_DIV1 = MXC_V_MCR_ADCCFG2_CH0_DIV1, ///< No input divider
    MXC_ADC_DIV2_5K =
        MXC_V_MCR_ADCCFG2_CH0_DIV2_5K, ///< Divide analog input by two with 5K resistor
    MXC_ADC_DIV2_50K =
        MXC_V_MCR_ADCCFG2_CH0_DIV2_50K, ///< Divide analog input by two with 50K resistor
} mxc_adc_divsel_t;

/**
 * @brief  Reference voltage select type.
 */
typedef enum {
    MXC_ADC_REF_EXT, ///< Use external reference voltage source
    MXC_ADC_REF_INT_1V25, ///< Use internal 1.25V source
    MXC_ADC_REF_INT_2V048, ///< Use internal 2.048V souce
} mxc_adc_refsel_t;

/**
 * @brief  Divide by 2 control in low power mode
 */

typedef enum {
    MXC_ADC_DIV_2_5K_50K_ENABLE, ///< 2.5K and 50K divide by 2 enable in lpmode
    MXC_ADC_DIV_2_5K_DISABLE, ///< 2.5K disable and 50K divide  by 2 enable in lpmode
    MXC_ADC_DIV_50K_DISABLE, ///< 2.5K enable and 50K divide  by 2 disable in lpmode
    MXC_ADC_DIV_2_5K_50K_DISABLE, ///< 2.5K and 50K divide by 2 disable in lpmode
} mxc_adc_div_lpmode_t;

/** TODO
 * @brief  Data FIFO data format
 */
typedef enum {
    MXC_ADC_DATA_STATUS, ///< Data(12-bit) plus Status
    MXC_ADC_DATA, ///< Data(12-bit) only
    MXC_ADC_RAW_DATA, ///< 18-bit raw data
} mxc_adc_fifodataformat_t;

/** TODO
 * @brief  Dynamic Divider pullup control
 */
typedef enum {
    MXC_ADC_PY_DN_DISABLE, ///< Disable Dynamic Divider Pullup
    MXC_ADC_PY_DN_ENABLE, ///< Enable Dynamic Divider Pullup
} mxc_adc_dynamic_pullup_t;

///< Callback used when a conversion event is complete
typedef void (*mxc_adc_complete_cb_t)(void *req, int error);

typedef struct {
    mxc_adc_clock_t clock; ///< clock to use
    mxc_adc_clkdiv_t clkdiv; ///< clock divider
    mxc_adc_calibration_t cal; ///< skip calibration
    mxc_adc_refsel_t ref; ///< ADC reference voltage
    uint32_t trackCount; ///< Sample Clock High time
    uint32_t idleCount; ///< Sample Clock Low time
} mxc_adc_req_t;

typedef struct {
    mxc_adc_chsel_t channel; ///< channel select
    mxc_adc_divsel_t div; ///< Analog input divider
    mxc_adc_dynamic_pullup_t pullup_dyn; ///< Dynamic Pullup
} mxc_adc_slot_req_t;

typedef struct {
    mxc_adc_conversion_mode_t mode; ///< conversion mode
    mxc_adc_trig_mode_t trig; ///< trigger mode
    mxc_adc_trig_sel_t hwTrig; ///< HW Trigger Source
    mxc_adc_fifodataformat_t fifo_format; ///< FIFO Data Format
    uint8_t fifo_threshold; ///< FIFO Threshold Configuration
    mxc_adc_avg_t avg_number; ///< no of samples to average
    mxc_adc_div_lpmode_t lpmode_divder; ///< Divide by 2 control in lpmode
    uint8_t num_slots; ///< num of slots in the sequence
} mxc_adc_conversion_req_t;

/**
 * @brief   Performs the ADC startup procedure
 *
 * @return  Success/Fail, see \ref MXC_Error_Codes for a list of return codes.
 */
int MXC_ADC_Init(mxc_adc_req_t *req);

/**
 * @brief   Shuts down the ADC
 *
 * @return  Success/Fail, see \ref MXC_Error_Codes for a list of return codes.
 */
int MXC_ADC_Shutdown(void);

/**
 * @brief   Enable specific ADC interrupts
 *
 * @param   flags mask of interrupt flags to enables
 */
void MXC_ADC_EnableInt(uint32_t flags);

/**
 * @brief   Disable specific ADC interrupts
 *
 * @param   flags mask of interrupt flags to enables
 */
void MXC_ADC_DisableInt(uint32_t flags);

/**
 * @brief   Performs the ADC startup procedure
 *
 * @return  active flags
 */
int MXC_ADC_GetFlags(void);

/**
 * @brief   Performs the ADC startup procedure
 *
 * @param   flags mask of flags to clear
 */
void MXC_ADC_ClearFlags(uint32_t flags);

/**
 * @brief   Initiate configured ADC conversion
 * @note    The channel must be configured separately
 *
 * @return  Raw conversion value, or \ref MXC_Error_Codes for error.
 */
int MXC_ADC_StartConversion(void);

/**
 * @brief   Perform a conversion on a specific channel
 * @note    The channel must be configured separately
 *          The ADC interrupt must be enabled and MXC_ADC_Handler() called in the ISR
 *          places data in the error parameter of the callback function
 *
 * @param   callback the function to call when the conversion is complete
 *
 * @return  Success/Fail, see \ref MXC_Error_Codes for a list of return codes.
 */
int MXC_ADC_StartConversionAsync(mxc_adc_complete_cb_t callback);

/**
 * @brief   Perform a conversion on a specific channel using a DMA transfer
 *
 * @param   req \ref mxc_adc_conversion_req_t
 * @param   pointer to the variable to hold the conversion result
 * @param   callback the function to call when the conversion is complete
 *
 * @return  Success/Fail, see \ref MXC_Error_Codes for a list of return codes.
 */
int MXC_ADC_StartConversionDMA(mxc_adc_conversion_req_t *req, int *data,
                               void (*callback)(int, int));

/**
 * @brief      Call this function from the ADC ISR when using Async API
 *             functions
 *
 * @return     Success/Fail, see \ref MXC_Error_Codes for a list of return codes.
 */
int MXC_ADC_Handler(void);

/**
 * @brief   Enable comparator hysteresis.
 *
 * @param   comp    Selects the comparator to enable hysteresis for.
 *
 * @return  Success/Fail, see \ref MXC_Error_Codes for a list of return codes.
 */
int MXC_ADC_ComparatorHysteresisEn(mxc_adc_comp_t comp);

/**
 * @brief   Disable comparator hysteresis.
 *
 * @param   comp    Selects the comparator to disable hysteresis for.
 *
 * @return  Success/Fail, see \ref MXC_Error_Codes for a list of return codes.
 */
int MXC_ADC_ComparatorHysteresisDis(mxc_adc_comp_t comp);

/**
 * @brief   Selects the analog input divider.
 *
 * @param   ch     Channel to set divider for
 * @param   div    The analog input divider to select
 * @param   lpEn   If Divide by 2, non-zero values will enable divide by 2 in low-power modes.
 *
 * @return  Success/Fail, see \ref MXC_Error_Codes for a list of return codes.
 */
int MXC_ADC_InputDividerSelect(mxc_adc_chsel_t ch, mxc_adc_divsel_t div,
                               mxc_adc_dynamic_pullup_t lpEn);

/**
 * @brief Selects the desired reference voltage for the ADC.
 *
 * @param ref   Desired reference voltage source.
 *
 * @return  Success/Fail, see \ref MXC_Error_Codes for a list of return codes.
 */
int MXC_ADC_ReferenceSelect(mxc_adc_refsel_t ref);

/**
 * @brief  Enables the ADC Dynamic Power-Up Mode.
 *
 * @param  Channel to enable dynamic mode for.
 *
 * @return  Success/Fail, see \ref MXC_Error_Codes for a list of return codes.
 */
int MXC_ADC_DynamicModeEn(mxc_adc_chsel_t ch);

/**
 * @brief  Disables the ADC Dynamic Power-Up Mode.
 *
 * @param  Channel to disable dynamic mode for.
 *
 * @return  Success/Fail, see \ref MXC_Error_Codes for a list of return codes.
 */
int MXC_ADC_DynamicModeDis(mxc_adc_chsel_t ch);

/**
 * @brief      Gets the ADC readout after the ADC conversion
 *
 * @param      Pointer to store the ADC data conversion result
 *
 * @return     see \ref MXC_Error_Codes for a list of return codes.
 */
int MXC_ADC_GetData(int *outdata);

/**
 * @brief      Configures the ADC.
 *
 * @param      pointer to the variable having ADC configuration.
 *
 * @return     see \ref MXC_Error_Codes for a list of return codes.
 */
int MXC_ADC_Configuration(mxc_adc_conversion_req_t *req);

/**
 * @brief      Configures ADC slot and channel registers.
 *
 * @param      Pointer of a variable having channel parameter configuration.
 * @param      Number of slots for ADC sequence.
 *
 * @return     see \ref MXC_Error_Codes for a list of return codes.
 */
int MXC_ADC_SlotConfiguration(mxc_adc_slot_req_t *req, uint32_t slot_length);

/**
 * @brief   Enables the temperature sensor
 *
 * @param   None
 */
void MXC_ADC_TS_SelectEnable(void);

/**
 * @brief   Disables the temperature sensor
 *
 * @param   None
 */
void MXC_ADC_TS_SelectDisable(void);

/**
 * @brief   Enables the ADC converter
 *
 * @param   None
 */
void MXC_ADC_EnableConversion(void);

/**
 * @brief   Disables the ADC converter
 *
 * @param   None
 */
void MXC_ADC_DisableConversion(void);

/**
 * @brief   Provides the ADC FIFO level.
 *
 * @return  return the ADC FIFO level counts.
 */
uint16_t MXC_ADC_FIFO_Level(void);

/**
 * @brief   Flushes the ADC FIFO.
 *
 * @param   None
 */
void MAX_ADC_Flush_FIFO(void);

/**
 * @brief   Configures the ADC FIFO threshold register.
 *
 * @param   FIFO threshold.
 *
 * @return  see \ref MXC_Error_Codes for a list of return codes.
 */
int MXC_ADC_FIFO_Threshold_Config(uint32_t fifo_threshold);

/**
 * @brief   Configures number of sample average in the sequence for each channel.
 *
 * @param   Average number
 *
 * @return  see \ref MXC_Error_Codes for a list of return codes.
 */
int MXC_ADC_AverageConfig(mxc_adc_avg_t avg_number);

/**
 * @brief   Clear all channel select registers.
 *
 * @return  None.
 */
void MXC_ADC_Clear_ChannelSelect(void);

/**
 * @brief   Configures ADC Trigger to initiate ADC conversion
 *
 * @param   pointer to ADC configure data structure.
 *
 * @return  None.
 */
void MXC_ADC_TriggerConfig(mxc_adc_conversion_req_t *req);

/**
 * @brief   Configures ADC Conversion Mode: Single vs Continuous Conversion.
 *
 * @param   pointer to ADC configure data structure.
 *
 * @return  None.
 */
void MXC_ADC_ConversionModeConfig(mxc_adc_conversion_req_t *req);

/**
 * @brief   Set Sample Delay before Continuous Mode Conversion Restart.
 *
 * @param   number of sample clock periods to delay in between conversions.
 *
 * @return  None.
 */
void MXC_ADC_SetConversionDelay(int delay);

/**
 * @brief   Configures number of slots for ADC sequence.
 *
 * @param   pointer to ADC configure data structure.
 *
 * @return  see \ref MXC_Error_Codes for a list of return codes.
 */
int MXC_ADC_SlotsConfig(mxc_adc_conversion_req_t *req);

/**
 * @brief   Calculates temperature (in K) from ADC readout.
 *
 * @param   Temperature readout from ADC.
 * @param   ADC refereence (internal 1.25V, 2.048V and external reference).
 * @param   external reference value for ADC if used.
 * @param   pointer to temperature value.
 *
 * @return  see \ref MXC_Error_Codes for a list of return codes.
 */
int MXC_ConvertTemperature_ToK(uint16_t tempSensor_Readout, mxc_adc_refsel_t ref, float ext_ref,
                               float *temp_k);

/**
 * @brief   Calculates temperature (in C) from ADC readout.
 *
 * @param   Temperature readout from ADC.
 * @param   ADC refereence (internal 1.25V, 2.048V and external reference).
 * @param   external reference value for ADC if used.
 * @param   pointer to temperature value.
 *
 * @return  see \ref MXC_Error_Codes for a list of return codes.
 */
int MXC_ConvertTemperature_ToC(uint16_t tempSensor_Readout, mxc_adc_refsel_t ref, float ext_ref,
                               float *temp);

/**
 * @brief   Calculates temperature (in F) from ADC readout.
 *
 * @param   Temperature readout from ADC.
 * @param   ADC refereence (internal 1.25V, 2.048V and external reference).
 * @param   external reference value for ADC if used.
 * @param   pointer to temperature value.
 *
 * @return  see \ref MXC_Error_Codes for a list of return codes.
 */
int MXC_ConvertTemperature_ToF(uint16_t tempSensor_Readout, mxc_adc_refsel_t ref, float ext_ref,
                               float *temp);

/**
 * @brief   Configures Divider (0-12 channel) in the low power mode.
 *
 * @param   Channel specific low power divider control in the lp mode.
 *
 * @return  see \ref MXC_Error_Codes for a list of return codes.
 */
int MXC_ADC_LowPowerModeDividerSelect(mxc_adc_div_lpmode_t div_lpmode);

/**@} end of group adc */

#ifdef __cplusplus
}
#endif

#endif // LIBRARIES_PERIPHDRIVERS_INCLUDE_MAX78002_ADC_H_
