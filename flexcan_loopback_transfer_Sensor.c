/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2021 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_debug_console.h"
#include "fsl_flexcan.h"
#include "fsl_gpio.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "gpio.h"
#include "dht11.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define EXAMPLE_CAN            CAN0
#define EXAMPLE_CAN_CLK_SOURCE (kFLEXCAN_ClkSrc1)
#define EXAMPLE_CAN_CLK_FREQ   CLOCK_GetFreq(kCLOCK_BusClk)
/* Set USE_IMPROVED_TIMING_CONFIG macro to use api to calculates the improved CAN / CAN FD timing values. */
#define USE_IMPROVED_TIMING_CONFIG (1U)
#define RX_MESSAGE_BUFFER_NUM      (9)
#define TX_MESSAGE_BUFFER_NUM      (8)
#define DLC                        (8)

/* Fix MISRA_C-2012 Rule 17.7. */
#define LOG_INFO (void)PRINTF
/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
volatile bool txComplete = false;
volatile bool rxComplete = false;
flexcan_handle_t flexcanHandle;
flexcan_mb_transfer_t txXfer, rxXfer;
#if (defined(USE_CANFD) && USE_CANFD)
flexcan_fd_frame_t txFrame, rxFrame;
#else
flexcan_frame_t txFrame, rxFrame;
#endif

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief FlexCAN Call Back function
 */
static FLEXCAN_CALLBACK(flexcan_callback)
{
    switch (status)
    {
        /* Process FlexCAN Rx event. */
        case kStatus_FLEXCAN_RxIdle:
            if (RX_MESSAGE_BUFFER_NUM == result)
            {
                rxComplete = true;
            }
            break;

        /* Process FlexCAN Tx event. */
        case kStatus_FLEXCAN_TxIdle:
            if (TX_MESSAGE_BUFFER_NUM == result)
            {
                txComplete = true;
            }
            break;

        default:
            break;
    }
}

void Configure_CAN_Pins ()
{
	PORT_SetPinMux(PORTB, 18, kPORT_MuxAlt2); //Tx
	PORT_SetPinMux(PORTB, 19, kPORT_MuxAlt2); //Rx

}

/*!
 * @brief Main function
 */
int main(void)
{
    flexcan_config_t flexcanConfig;
    flexcan_rx_mb_config_t mbConfig;
    uint32_t rx0;
    uint32_t rx1;

    /* Initialize board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();
    Configure_CAN_Pins ();
    GPIO_init();
    dht_data_t data2send = {0};
    uint32_t clk_freq = CLOCK_GetCoreSysClkFreq();
    LOG_INFO("\r\n==FlexCAN loopback example -- Start.==\r\n\r\n");

    /* Init FlexCAN module. */
    /*
     * flexcanConfig.clkSrc                 = kFLEXCAN_ClkSrc0;
     * flexcanConfig.baudRate               = 1000000U;
     * flexcanConfig.baudRateFD             = 2000000U;
     * flexcanConfig.maxMbNum               = 16;
     * flexcanConfig.enableLoopBack         = false;
     * flexcanConfig.enableSelfWakeup       = false;
     * flexcanConfig.enableIndividMask      = false;
     * flexcanConfig.disableSelfReception   = false;
     * flexcanConfig.enableListenOnlyMode   = false;
     * flexcanConfig.enableDoze             = false;
     */
    FLEXCAN_GetDefaultConfig(&flexcanConfig);

#if defined(EXAMPLE_CAN_CLK_SOURCE)
    flexcanConfig.clkSrc = EXAMPLE_CAN_CLK_SOURCE;
#endif


#if (defined(USE_IMPROVED_TIMING_CONFIG) && USE_IMPROVED_TIMING_CONFIG)
    flexcan_timing_config_t timing_config;
    memset(&timing_config, 0, sizeof(flexcan_timing_config_t));

    if (FLEXCAN_CalculateImprovedTimingValues(EXAMPLE_CAN, flexcanConfig.baudRate, EXAMPLE_CAN_CLK_FREQ,
                                              &timing_config))
    {
        /* Update the improved timing configuration*/
        memcpy(&(flexcanConfig.timingConfig), &timing_config, sizeof(flexcan_timing_config_t));
    }
    else
    {
        LOG_INFO("No found Improved Timing Configuration. Just used default configuration\r\n\r\n");
    }
#endif

    FLEXCAN_Init(EXAMPLE_CAN, &flexcanConfig, EXAMPLE_CAN_CLK_FREQ);


    /* Setup Rx Message Buffer. */
    mbConfig.format = kFLEXCAN_FrameFormatStandard;
    mbConfig.type   = kFLEXCAN_FrameTypeData;
    mbConfig.id     = FLEXCAN_ID_STD(0x8);

    FLEXCAN_SetRxMbConfig(EXAMPLE_CAN, RX_MESSAGE_BUFFER_NUM, &mbConfig, true);


/* Setup Tx Message Buffer. */

    FLEXCAN_SetTxMbConfig(EXAMPLE_CAN, TX_MESSAGE_BUFFER_NUM, true);

    /* Create FlexCAN handle structure and set call back function. */
    FLEXCAN_TransferCreateHandle(EXAMPLE_CAN, &flexcanHandle, flexcan_callback, NULL);

while(1)
{
    /* Prepare Tx Frame for sending. */
    txFrame.format = (uint8_t)kFLEXCAN_FrameFormatStandard;
    txFrame.type   = (uint8_t)kFLEXCAN_FrameTypeData;
    txFrame.id     = FLEXCAN_ID_STD(0x4);
    txFrame.length = (uint8_t)DLC;

    data2send = dht11_read();


    txFrame.dataWord0 = CAN_WORD0_DATA_BYTE_3(data2send.temperature);
    txFrame.dataWord1 = CAN_WORD1_DATA_BYTE_7(data2send.humidity);


    LOG_INFO("Send message from MB%d to MB%d\r\n", TX_MESSAGE_BUFFER_NUM, RX_MESSAGE_BUFFER_NUM);

    LOG_INFO("tx word0 = 0x%x\r\n", txFrame.dataWord0);
    LOG_INFO("tx word1 = 0x%x\r\n", txFrame.dataWord1);


    /* Send data through Tx Message Buffer. */
    txXfer.mbIdx = (uint8_t)TX_MESSAGE_BUFFER_NUM;

    txXfer.frame = &txFrame;
    (void) FLEXCAN_TransferSendNonBlocking(EXAMPLE_CAN, &flexcanHandle, &txXfer);

//    /* Start receive data through Rx Message Buffer. */
//    rxXfer.mbIdx = (uint8_t)RX_MESSAGE_BUFFER_NUM;
//
//    rxXfer.frame = &rxFrame;
//    (void)FLEXCAN_TransferReceiveNonBlocking(EXAMPLE_CAN, &flexcanHandle, &rxXfer);

    /* Waiting for Rx Message finish. */
    while ((!txComplete))
    {
    };

    txComplete = false;


    rx0 = rxFrame.dataWord0;
    rx1 = rxFrame.dataWord1;
    LOG_INFO("rx word0 = 0x%x\r\n", rx0);
    LOG_INFO("rx word1 = 0x%x\r\n", rx1);

    SDK_DelayAtLeastUs(10000,clk_freq);

    txFrame.format = (uint8_t)kFLEXCAN_FrameFormatStandard;
    txFrame.type   = (uint8_t)kFLEXCAN_FrameTypeData;
    txFrame.id     = FLEXCAN_ID_STD(0x6);
    txFrame.length = (uint8_t)DLC;

    txFrame.dataWord0 = CAN_WORD0_DATA_BYTE_3(data2send.temperature);
    txFrame.dataWord1 = CAN_WORD1_DATA_BYTE_7(data2send.humidity);

    txXfer.mbIdx = (uint8_t)TX_MESSAGE_BUFFER_NUM;

    txXfer.frame = &txFrame;

    (void) FLEXCAN_TransferSendNonBlocking(EXAMPLE_CAN, &flexcanHandle, &txXfer);
    while ((!txComplete))
    {
    };

    txComplete = false;


//    LOG_INFO("\r\n==FlexCAN loopback example -- Finish.==\r\n");

    SDK_DelayAtLeastUs(10000,clk_freq);

    txFrame.format = (uint8_t)kFLEXCAN_FrameFormatStandard;
    txFrame.type   = (uint8_t)kFLEXCAN_FrameTypeData;
    txFrame.id     = FLEXCAN_ID_STD(0x8);
    txFrame.length = (uint8_t)DLC;

    txFrame.dataWord0 = CAN_WORD0_DATA_BYTE_3(data2send.temperature);
    txFrame.dataWord1 = CAN_WORD1_DATA_BYTE_7(data2send.humidity);

    txXfer.mbIdx = (uint8_t)TX_MESSAGE_BUFFER_NUM;

    txXfer.frame = &txFrame;

    (void) FLEXCAN_TransferSendNonBlocking(EXAMPLE_CAN, &flexcanHandle, &txXfer);
    while ((!txComplete))
    {
    };

    txComplete = false;

uint8_t counter = 0;
SDK_DelayAtLeastUs(1000000,clk_freq);
}

LOG_INFO("\r\n==FlexCAN loopback example -- Finish.==\r\n");

}


