/*
Copyright 2016 Mikael Degerfält <mikae.degerfalt@gmail.com>

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
#include <avr/pgmspace.h>
#include "keycode.h"
#include "action.h"
#include "util.h"
#include "serial.h"
#include "keymap.h"


// Following macros help you to define a keymap with the form of actual keyboard layout.

/* Atari ST Keyboard Scan codes
     ,------------------------------------------------------------.
    /  3b /  3c /  3d /  3e /  3f /  40 /  41 /  42 /  43 /  44  /
    `-----------------------------------------------------------'
    ,-----------------------------------------------------------.  ,-----------. ,---------------.
    | 01| 02| 03| 04| 05| 06| 07| 08| 09| 0A| 0B| 0C| 0D| 29| 0E|  |  62 |  61 | | 63| 64| 65| 66|
    |-----------------------------------------------------------|  |------------ |---------------|
    | 0F | 10| 11| 12| 13| 14| 15| 16| 17| 18| 19| 1A| 1B|  | 53|  | 52| 48| 47| | 67| 68| 69| 4A|
    |----------------------------------------------------´  |---|  |-----------| |---------------|
    |  1D  | 1E| 1F| 20| 21| 22| 23| 24| 25| 26| 27| 28| 1C | 2B|  | 4B| 50| 4D| | 6A| 6B| 6C| 4E|
    |-----------------------------------------------------------'  `-----------' |-----------|---|
    |  2A| 60| 2C| 2D| 2E| 2F| 30| 31| 32| 33| 34| 35|  36|                      | 6D| 6E| 6F|   |
    `-----------------------------------------------------'                      |-----------| 72|
         |   38|                     39            |  3A|                        |     70| 71|   |
         `----------------------------------------------'                        `---------------'
*/
#define KEYMAP( \
    K3B,K3C,K3D,K3E,K3F,K40,K41,K42,K43,K44,                                                     \
    K01,K02,K03,K04,K05,K06,K07,K08,K09,K0A,K0B,K0C,K0D,K29,K0E,    K62, K61,   K63,K64,K65,K66, \
    K0F,K10,K11,K12,K13,K14,K15,K16,K17,K18,K19,K1A,K1B,K1C,K53,  K52,K48,K47,  K67,K68,K69,K4A, \
    K1D,K1E,K1F,K20,K21,K22,K23,K24,K25,K26,K27,K28,        K2B,  K4B,K50,K4D,  K6A,K6B,K6C,K4E, \
    K2A,K60,K2C,K2D,K2E,K2F,K30,K31,K32,K33,K34,K35,K36,                        K6D,K6E,K6F,K72, \
        K38,                   K39,             K3A,                            K70,    K71      \
) { \
    { KC_NO,    KC_##K01, KC_##K02, KC_##K03, KC_##K04, KC_##K05, KC_##K06, KC_##K07 }, \
    { KC_##K08, KC_##K09, KC_##K0A, KC_##K0B, KC_##K0C, KC_##K0D, KC_##K0E, KC_##K0F }, \
    { KC_##K10, KC_##K11, KC_##K12, KC_##K13, KC_##K14, KC_##K15, KC_##K16, KC_##K17 }, \
    { KC_##K18, KC_##K19, KC_##K1A, KC_##K1B, KC_##K1C, KC_##K1D, KC_##K1E, KC_##K1F }, \
    { KC_##K20, KC_##K21, KC_##K22, KC_##K23, KC_##K24, KC_##K25, KC_##K26, KC_##K27 }, \
    { KC_##K28, KC_##K29, KC_##K2A, KC_##K2B, KC_##K2C, KC_##K2D, KC_##K2E, KC_##K2F }, \
    { KC_##K30, KC_##K31, KC_##K32, KC_##K33, KC_##K34, KC_##K35, KC_##K36, KC_NO    }, \
    { KC_##K38, KC_##K39, KC_##K3A, KC_##K3B, KC_##K3C, KC_##K3D, KC_##K3E, KC_##K3F }, \
    { KC_##K40, KC_##K41, KC_##K42, KC_##K43, KC_##K44, KC_NO,    KC_NO,    KC_##K47 }, \
    { KC_##K48, KC_NO,    KC_##K4A, KC_##K4B, KC_NO,    KC_##K4D, KC_##K4E, KC_NO    }, \
    { KC_##K50, KC_NO,    KC_##K52, KC_##K53, KC_NO,    KC_NO,    KC_NO,    KC_NO    }, \
    { KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO    }, \
    { KC_##K60, KC_##K61, KC_##K62, KC_##K63, KC_##K64, KC_##K65, KC_##K66, KC_##K67 }, \
    { KC_##K68, KC_##K69, KC_##K6A, KC_##K6B, KC_##K6C, KC_##K6D, KC_##K6E, KC_##K6F }, \
    { KC_##K70, KC_##K71, KC_##K72, KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO    }, \
    { KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO    }  \
}


// Don't know what this is for...
static const uint16_t fn_actions[] PROGMEM = {
    [0] = ACTION_FUNCTION(0),   // toggle all LEDs
};

void action_function(keyrecord_t *record, uint8_t id, uint8_t opt)
{
    return;
    /*
    static bool led = false;
    switch (id) {
        case 0:
            if (record->event.pressed) {
                if ((led = !led))
                    serial_send(0x80);  // all on
                else
                    serial_send(0xff);  // all off
            }
            break;
    }
    */
}


static const uint8_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
/* Atari ST Keyboard Scan codes
     ,------------------------------------------------------------.
    /  3b /  3c /  3d /  3e /  3f /  40 /  41 /  42 /  43 /  44  /
    `-----------------------------------------------------------'
    ,-----------------------------------------------------------.  ,-----------. ,---------------.
    | 01| 02| 03| 04| 05| 06| 07| 08| 09| 0A| 0B| 0C| 0D| 29| 0E|  |  62 |  61 | | 63| 64| 65| 66|
    |-----------------------------------------------------------|  |------------ |---------------|
    | 0F | 10| 11| 12| 13| 14| 15| 16| 17| 18| 19| 1A| 1B|  | 53|  | 52| 48| 47| | 67| 68| 69| 4A|
    |----------------------------------------------------´  |---|  |-----------| |---------------|
    |  1D  | 1E| 1F| 20| 21| 22| 23| 24| 25| 26| 27| 28| 1C | 2B|  | 4B| 50| 4D| | 6A| 6B| 6C| 4E|
    |-----------------------------------------------------------'  `-----------' |-----------|---|
    |  2A| 60| 2C| 2D| 2E| 2F| 30| 31| 32| 33| 34| 35|  36|                      | 6D| 6E| 6F|   |
    `-----------------------------------------------------'                      |-----------| 72|
         |   38|                     39            |  3A|                        |     70| 51|   |
         `----------------------------------------------'                        `---------------'
*/
    /* UK? */
    KEYMAP(
    F1,  F2,  F3, F4, F5, F6, F7, F8, F9, F10,
    ESC,  1,   2,  3,  4,  5,  6,  7,  8,  9,   0, MINS, EQL,BSLS,BSPC,   HELP,     UNDO,      NLCK,PSLS,PSLS,PAST,
    TAB,  Q,   W,  E,  R,  T,  Y,  U,  I,  O,   P, LBRC,RBRC, ENT, DEL,   INS,   UP, CLR,      P7,  P8,  P9,  PMNS,
    LCTL, A,   S,  D,  F,  G,  H,  J,  K,  L,SCLN, QUOT,          NUHS,   LEFT,DOWN,RGHT,      P4,  P5,  P6,  PPLS,
    LSFT,BSLS, Z,  X,  C,  V,  B,  N,  M,  COMM,DOT, SLSH,RSFT,                                P1,  P2,  P3,  PENT,
         LALT,                 SPC,      CLCK,                                                 P0,       PDOT
    ),
};


/* translates key to keycode */
uint8_t keymap_key_to_keycode(uint8_t layer, keypos_t key)
{
    return pgm_read_byte(&keymaps[(layer)][(key.row)][(key.col)]);
}

/* translates Fn keycode to action */
action_t keymap_fn_to_action(uint8_t keycode)
{
    return (action_t){ .code = pgm_read_word(&fn_actions[FN_INDEX(keycode)]) };
}
