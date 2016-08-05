/*
Copyright 2016 Mikael Degerfält <mikael.degerfalt@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CONFIG_H
#define CONFIG_H


#define VENDOR_ID       0xDE31
#define PRODUCT_ID      0xA570
#define DEVICE_VER      0x0002
#define MANUFACTURER    vulkteamet
#define PRODUCT         ATARI IKBD keyboard converter
#define DESCRIPTION     converts Atari ST keyboard protocol into USB


/* PORTS usage for led and reset pin to IKBD */
#define IKDB_LED_PORT_REG PORTB
#define IKDB_LED_DD_REG DDRB
#define IKDB_LED_BIT_MASK (1u<<(1))

#define IKDB_RST_PORT_REG PORTB
#define IKDB_RST_DD_REG DDRB
#define IKDB_RST_BIT_MASK (1u<<(3))

/* matrix size */
#define MATRIX_ROWS 16
#define MATRIX_COLS 8

#define IS_COMMAND() false


/* key combination for command
#define IS_COMMAND() ( \
    keyboard_report->mods == (MOD_BIT(KC_LALT) | MOD_BIT(KC_RALT)) || \
    keyboard_report->mods == (MOD_BIT(KC_LGUI) | MOD_BIT(KC_RGUI)) || \
    keyboard_report->mods == (MOD_BIT(KC_LCTL) | MOD_BIT(KC_RCTL)) \
)
*/

/* USART configuration
 *     asynchronous, 7812.5baud, 8-data bit, no parity, 1-stop bit, no flow control
 */
#if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega32U2__)
    #define SERIAL_UART_BAUD       7812.5
    #define SERIAL_UART_DATA       UDR1
    #define SERIAL_UART_UBRR       (uint16_t)((F_CPU/(16UL*SERIAL_UART_BAUD))-1)
    #define SERIAL_UART_RXD_VECT   USART1_RX_vect
    #define SERIAL_UART_TXD_READY  (UCSR1A&(1<<UDRE1))
    #define SERIAL_UART_INIT()     do { \
        UBRR1L = (uint8_t) SERIAL_UART_UBRR;       /* baud rate */ \
        UBRR1H = (uint8_t) (SERIAL_UART_UBRR>>8);  /* baud rate */ \
        UCSR1B = (1<<RXCIE1) | (1<<RXEN1) | /* RX: interrupt, RX: enable */ \
                 (0<<UCSZ12) | (1<<TXEN1);                /* TX: enable */ \
        UCSR1C = (0<<UPM11) | (0<<UPM10) | /* parity: none(00), even(01), odd(11) */ \
                 (1<<UCSZ11) | (1<<UCSZ10); /* data-8bit(011) */ \
        sei(); \
    } while(0)
#else
#   error "USART configuration is needed."
#endif

#endif
