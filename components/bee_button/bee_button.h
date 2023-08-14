/*****************************************************************************
 *
 * @file 	bee_button.h
 * @author 	tuha
 * @date 	3 July 2023
 * @brief	module for project cotrol by button
 *
 ***************************************************************************/
#ifndef BEE_BUTTON_H
#define BEE_BUTTON_H

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#define SEND_DATA_BUTTON        GPIO_NUM_32
#define INTERVAL_BUTTON         GPIO_NUM_0

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/**
 * @brief Initialize buttons and their ISR handlers.
 *
 * This function initializes the buttons used in the application and sets up their ISR handlers. It installs the ISR
 * service for GPIO interrupts, adds ISR handlers for the SEND_DATA_BUTTON and INTERVAL_BUTTON, configures the GPIO pins,
 * sets the pull-up mode, and specifies the interrupt type. It also creates event queues for button events.
 */
void button_init(void);

/**
 * @brief Task function to handle the SEND_DATA_BUTTON.
 *
 * This task function monitors the event queue for the SEND_DATA_BUTTON and toggles the send_data flag when the button is clicked.
 * It also saves the updated flag to NVS storage.
 *
 * @param arg Pointer to any task-specific data (not used in this function).
 */
void send_data_button_isr(void* arg);

/**
 * @brief Task function to handle the INTERVAL_BUTTON.
 *
 * This task function monitors the event queue for the INTERVAL_BUTTON and cycles through different interval settings when the button is clicked.
 * It updates the `u8data_interval_uart` variable and saves the updated interval to NVS storage.
 *
 * @param arg Pointer to any task-specific data (not used in this function).
 */
void interval_button_isr(void* arg);

#endif
