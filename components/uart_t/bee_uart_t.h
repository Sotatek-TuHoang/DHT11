/*****************************************************************************
 *
 * @file 	bee_Uart.c
 * @author 	biennd
 * @date 	13 Jun 2023
 * @brief	All functions relate to uart
 *
 ***************************************************************************/


#ifndef UART_H
#define UART_H
#include <stdint.h>

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#define EX_UART_NUM UART_NUM_2
#define PATTERN_CHR_NUM    (3)         /*!< Set the number of consecutive and identical characters received by receiver which defines a UART pattern*/
#define BUF_SIZE (1024)
#define RD_BUF_SIZE (BUF_SIZE)


/****************************************************************************/
/***        Type Definitions - Callback                                               ***/
/****************************************************************************/
typedef void (*uart_handle_t) (uint8_t *data, uint16_t length);

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
/** @brief Init Uart
 *  @param void
 *  @return void
 */
void uart_init_cmd(void);

/****************************************************************************/
/** @brief Uart handle Callback
 *  @param void
 *  @return void
 */
/****************************************************************************/
void uart_set_callback(void *cb);

/****************************************************************************/
/** @brief Send data
 *  @param dta - data to send
 *  @param len - length of data to send
 *  @return void
 */
/****************************************************************************/
void uart_put( char *dta, int len);



#endif