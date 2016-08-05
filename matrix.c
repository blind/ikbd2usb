/*
Copyright 2012 Jun Wako <wakojun@gmail.com>

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

#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <util/delay.h>
#include "print.h"
#include "util.h"
#include "serial.h"
#include "matrix.h"
#include "debug.h"

static uint8_t handle_incoming_data(void);
static uint8_t wait_for_ikbd(void);
static void update_led(void);

/*
 * Matrix Array usage:
 *
 * ROW: 16(4bits)
 * COL:  8(3bits)
 *
 *    8bit wide
 *   +---------+
 *  0|00 ... 07|
 *  1|08 ... 0F|
 *  :|   ...   |
 *  :|   ...   |
 *  E|70 ... 77|
 *  F|78 ... 7F|
 *   +---------+
 *
 */
static uint8_t matrix[MATRIX_ROWS];
#define ROW(code)      ((code>>3)&0xF)
#define COL(code)      (code&0x07)

static bool is_modified = false;


#define IKBD_STATE_IDLE 0
#define IKBD_STATE_WAITING 1
#define IKBD_STATE_STARTUP 2


static uint8_t event_buffer[8];
static uint8_t event_buffer_index = 0;
static uint8_t ikbd_state = IKBD_STATE_STARTUP;

// led blinking stuff
static uint16_t led_counter;
static uint16_t led_period = 50;
static uint16_t last_timer;

inline
uint8_t matrix_rows(void)
{
    return MATRIX_ROWS;
}

inline
uint8_t matrix_cols(void)
{
    return MATRIX_COLS;
}

void matrix_init(void)
{
    serial_init();

    last_timer = timer_read();
    /* Init reset and led as ouput  */
    IKDB_RST_DD_REG |= IKDB_RST_BIT_MASK;
    IKDB_LED_DD_REG |= IKDB_LED_BIT_MASK;

    // initialize matrix state: all keys off
    for (uint8_t i=0; i < MATRIX_ROWS; i++) {
        matrix[i] = 0x00;
    }

    /* sleep for a while before waking the keyboard from
        reset
     */
    _delay_ms(100);
    IKDB_RST_PORT_REG |= IKDB_RST_BIT_MASK;

    // Send reset command
    serial_send(0x80);
    serial_send(0x01);
}

uint8_t matrix_scan(void)
{
    switch( ikbd_state )
    {
    case IKBD_STATE_STARTUP:
        ikbd_state = wait_for_ikbd();
        break;
    case IKBD_STATE_IDLE:
        led_period = 500;
        ikbd_state = handle_incoming_data();
        break;
    default:
        led_period = 1000;
        ikbd_state = handle_incoming_data();
        break;
    }

//    ikbd_state = handle_incoming_data();
    update_led();
    return 0;
}


bool matrix_is_modified(void)
{
    return is_modified;
}

inline
bool matrix_has_ghost(void)
{
    return false;
}

inline
bool matrix_is_on(uint8_t row, uint8_t col)
{
    return (matrix[row] & (1<<col));
}

inline
uint8_t matrix_get_row(uint8_t row)
{
    return matrix[row];
}

void matrix_print(void)
{
    print("\nr/c 01234567\n");
    for (uint8_t row = 0; row < matrix_rows(); row++) {
        phex(row); print(": ");
        pbin_reverse(matrix_get_row(row));
        print("\n");
    }
}

uint8_t matrix_key_count(void)
{
    uint8_t count = 0;
    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
        count += bitpop(matrix[i]);
    }
    return count;
}


// Local functions...

static uint8_t wait_for_ikbd()
{
    uint16_t code;
    code = serial_recv2();
    if (code == -1)
        return IKBD_STATE_STARTUP;

    led_period = 5000;

    if (code & 0x80u){
        if (code < 0xf0) {
            // Break code received which indicates
            // keyboard error
            dprintf( "Got BREAK code at startup, stuck key detected" );
            return IKBD_STATE_IDLE;
        } else {
            dprintf( "IKBD boot ok, version $%02X", code&0x0f );
            // Tell IKBD to not send mouse events
            led_period = 100;
            serial_send(0x12);
            return IKBD_STATE_IDLE;
        }
    } else {
        dprintf( "Got MAKE code at startup, somethings weird" );
        return IKBD_STATE_IDLE;
    }
    return IKBD_STATE_STARTUP;
}


static void update_led()
{
    uint16_t diff = timer_elapsed( last_timer );
    led_counter += diff;
    while( led_counter > led_period )
    {
        led_counter -= led_period;
        IKDB_LED_DD_REG ^= IKDB_LED_BIT_MASK;
    }
    last_timer = timer_read();
}

static uint8_t handle_incoming_data()
{
    is_modified = false;

    uint16_t code;
    code = serial_recv2();
    if (code == -1) {
        return 0;
    }

    led_period = 200;
    dprintf("%02X\n", code);

    // For now, just ignore status reports, clock and mouse/joystick events
    if (code >= 0xf6)
        return 0;

    if (code&0x80) {
        // break code
        if (matrix_is_on(ROW(code), COL(code))) {
            matrix[ROW(code)] &= ~(1<<COL(code));
            is_modified = true;
        }
    } else {
        // make code
        if (!matrix_is_on(ROW(code), COL(code))) {
            matrix[ROW(code)] |=  (1<<COL(code));
            is_modified = true;
        }
    }
    return code;
}
