/**
 * Copyright 2022 Charly Delay <charly@codesink.dev> (@0xcharly)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include QMK_KEYBOARD_H

#ifdef DILEMMA_AUTO_POINTER_LAYER_TRIGGER_ENABLE
#    include "timer.h"
#endif // DILEMMA_AUTO_POINTER_LAYER_TRIGGER_ENABLE

// Layer enum
enum dilemma_keymap_layers {
    LAYER_BASE = 0,
    LAYER_NUMERAL,
    LAYER_SYMBOLS,
    LAYER_NAVIGATION,
    LAYER_POINTER,
};

// Double-tap OSM Shift for Caps Word
static uint16_t last_shift_tap_time = 0;
#define DOUBLE_TAP_TERM 250  // ms window for double-tap

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        // Check for OSM Shift keys (both left and right)
        if (keycode == OSM(MOD_LSFT) || keycode == OSM(MOD_RSFT)) {
            if (timer_elapsed(last_shift_tap_time) < DOUBLE_TAP_TERM) {
                // Double-tap detected - toggle caps word
                caps_word_toggle();
                last_shift_tap_time = 0;  // Reset to prevent triple-tap
                return false;  // Don't process the shift
            }
            last_shift_tap_time = timer_read();
        }
    }
    return true;
}




enum combos {
    // Utility combos
    QW_ESC = 0,
    ER_TAB,
    UI_DEL,
    OP_BKSPC,
    QP_BASE,       // Reset to base layer (base layer keys)
    QP_BASE_NUM,   // Reset from NUM layer (F1 + -)
    QP_BASE_SYM,   // Reset from SYM layer (! + `)
    // Left hand modifiers (Callum-style one-shot)
    AS_SHFT,
    SD_CTRL,
    DF_ALT,
    ASD_GUI,
    ZC_MEH,
    // Right hand modifiers (Callum-style one-shot)
    LQUOT_SHFT,
    KL_CTRL,
    JK_ALT,
    KLQ_GUI,
    COMSLSH_MEH,
};

// Utility combos
const uint16_t PROGMEM qw_combo[]          = {KC_Q, KC_W, COMBO_END};
const uint16_t PROGMEM er_combo[]          = {KC_E, KC_R, COMBO_END};
const uint16_t PROGMEM ui_combo[]          = {KC_U, KC_I, COMBO_END};
const uint16_t PROGMEM op_combo[]          = {KC_O, KC_P, COMBO_END};
const uint16_t PROGMEM qp_combo[]          = {KC_Q, KC_P, COMBO_END};
const uint16_t PROGMEM qp_combo_num[]      = {KC_F1, KC_MINS, COMBO_END};   // Q+P positions on NUM
const uint16_t PROGMEM qp_combo_sym[]      = {KC_EXLM, KC_GRV, COMBO_END};  // Q+P positions on SYM
// Left hand modifier combos
const uint16_t PROGMEM as_combo[]      = {KC_A, KC_S, COMBO_END};
const uint16_t PROGMEM sd_combo[]      = {KC_S, KC_D, COMBO_END};
const uint16_t PROGMEM df_combo[]      = {KC_D, KC_F, COMBO_END};
const uint16_t PROGMEM asd_combo[]     = {KC_A, KC_S, KC_D, COMBO_END};
const uint16_t PROGMEM zc_combo[]      = {KC_Z, KC_C, COMBO_END};
// Right hand modifier combos
const uint16_t PROGMEM lquot_combo[]   = {KC_L, KC_QUOT, COMBO_END};
const uint16_t PROGMEM kl_combo[]      = {KC_K, KC_L, COMBO_END};
const uint16_t PROGMEM jk_combo[]      = {KC_J, KC_K, COMBO_END};
const uint16_t PROGMEM klq_combo[]     = {KC_K, KC_L, KC_QUOT, COMBO_END};
const uint16_t PROGMEM comslsh_combo[] = {KC_COMM, KC_SLSH, COMBO_END};

combo_t key_combos[] = {
    // Utility
    [QW_ESC]      = COMBO(qw_combo, KC_ESC),
    [ER_TAB]      = COMBO(er_combo, KC_TAB),
    [UI_DEL]      = COMBO(ui_combo, KC_DEL),
    [OP_BKSPC]    = COMBO(op_combo, KC_BSPC),
    [QP_BASE]     = COMBO(qp_combo, TO(LAYER_BASE)),      // Q+P on base
    [QP_BASE_NUM] = COMBO(qp_combo_num, TO(LAYER_BASE)),  // F1+- on NUM
    [QP_BASE_SYM] = COMBO(qp_combo_sym, TO(LAYER_BASE)),  // !+` on SYM
    // Left hand modifiers
    [AS_SHFT]     = COMBO(as_combo, OSM(MOD_LSFT)),
    [SD_CTRL]     = COMBO(sd_combo, OSM(MOD_LCTL)),
    [DF_ALT]      = COMBO(df_combo, OSM(MOD_LALT)),
    [ASD_GUI]     = COMBO(asd_combo, OSM(MOD_LGUI)),
    [ZC_MEH]      = COMBO(zc_combo, OSM(MOD_MEH)),
    // Right hand modifiers
    [LQUOT_SHFT]  = COMBO(lquot_combo, OSM(MOD_RSFT)),
    [KL_CTRL]     = COMBO(kl_combo, OSM(MOD_RCTL)),
    [JK_ALT]      = COMBO(jk_combo, OSM(MOD_RALT)),
    [KLQ_GUI]     = COMBO(klq_combo, OSM(MOD_RGUI)),
    [COMSLSH_MEH] = COMBO(comslsh_combo, OSM(MOD_MEH)),
};

// Automatically enable sniping-mode on the pointer layer.
#define DILEMMA_AUTO_SNIPING_ON_LAYER LAYER_POINTER

#ifdef DILEMMA_AUTO_POINTER_LAYER_TRIGGER_ENABLE
static uint16_t auto_pointer_layer_timer = 0;

#    ifndef DILEMMA_AUTO_POINTER_LAYER_TRIGGER_TIMEOUT_MS
#        define DILEMMA_AUTO_POINTER_LAYER_TRIGGER_TIMEOUT_MS 1000
#    endif // DILEMMA_AUTO_POINTER_LAYER_TRIGGER_TIMEOUT_MS

#    ifndef DILEMMA_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD
#        define DILEMMA_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD 8
#    endif // DILEMMA_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD
#endif     // DILEMMA_AUTO_POINTER_LAYER_TRIGGER_ENABLE

#define _L_PTR(KC) LT(LAYER_POINTER, KC)

#ifndef POINTING_DEVICE_ENABLE
#    define DRGSCRL KC_NO
#    define DPI_MOD KC_NO
#    define S_D_MOD KC_NO
#    define SNIPING KC_NO
#endif // !POINTING_DEVICE_ENABLE

// clang-format off
/** \brief Base layer - pure QWERTY, no mod-taps. Modifiers via combos. */
#define LAYOUT_LAYER_BASE                                                                     \
       KC_Q, KC_W, KC_E, KC_R, KC_T,      KC_Y, KC_U, KC_I,    KC_O,   KC_P,    \
       KC_A, KC_S, KC_D, KC_F, KC_G,      KC_H, KC_J, KC_K,    KC_L,   KC_QUOT, \
       KC_Z, KC_X, KC_C, KC_V, KC_B,      KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, \
             OSL(LAYER_NUMERAL), KC_SPC,  KC_ENT, OSL(LAYER_SYMBOLS)

/** Convenience row shorthands. */
#define _______________DEAD_HALF_ROW_______________ XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX
#define ______________HOME_ROW_GACS_L______________ KC_LGUI, KC_LALT, KC_LCTL, KC_LSFT, XXXXXXX
#define ______________HOME_ROW_GACS_R______________ XXXXXXX, KC_RSFT, KC_RCTL, KC_RALT, KC_RGUI

/*
 * Layers used on the Dilemma.
 *
 * These layers started off heavily inspired by the Miryoku layout, but trimmed
 * down and tailored for a stock experience that is meant to be fundation for
 * further personalization.
 *
 * See https://github.com/manna-harbour/miryoku for the original layout.
 */

/** \brief Mouse emulation and pointer functions. */
#define LAYOUT_LAYER_POINTER                                                                  \
    QK_BOOT, XXXXXXX, XXXXXXX, DPI_MOD, S_D_MOD, S_D_MOD, DPI_MOD, XXXXXXX, XXXXXXX, QK_BOOT, \
    ______________HOME_ROW_GACS_L______________, ______________HOME_ROW_GACS_R______________, \
    _______, DRGSCRL, SNIPING, KC_BTN3, XXXXXXX, XXXXXXX, KC_BTN3, SNIPING, DRGSCRL, _______, \
                               KC_BTN2, KC_BTN1, KC_BTN1, KC_BTN2

/**
 * \brief Navigation layer - vim-style HJKL arrows.
 */
#define LAYOUT_LAYER_NAVIGATION                                                               \
    _______________DEAD_HALF_ROW_______________, _______________DEAD_HALF_ROW_______________, \
    _______________DEAD_HALF_ROW_______________, KC_LEFT, KC_DOWN,   KC_UP, KC_RGHT, KC_CAPS, \
    _______________DEAD_HALF_ROW_______________,  KC_HOME, KC_PGDN, KC_PGUP,  KC_END, KC_INS, \
                               _______, _______,  KC_ENT, KC_BSPC

/**
 * \brief Numeral layer - F-keys on left, numpad on right.
 * 0 on thumb, QK_REP and QK_AREP on bottom left for easy repeat access.
 */
#define LAYOUT_LAYER_NUMERAL                                                                  \
      KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,    KC_SLSH,   KC_7,    KC_8,    KC_9, KC_MINS, \
      KC_F6,   KC_F7,   KC_F8,   KC_F9,  KC_F10,    KC_ASTR,   KC_4,    KC_5,    KC_6, KC_PLUS, \
     KC_F11,  KC_F12,  QK_REP, QK_AREP, CW_TOGG,     KC_EQL,   KC_1,    KC_2,    KC_3,  KC_DOT, \
                               XXXXXXX, _______,    _______,    KC_0

/**
 * \brief Symbols layer - symmetrical brackets for programming.
 *
 * Home row has paired brackets: < { [ ( on left, ) ] } > on right.
 * - and = on index fingers for easy ->, =>, -=, +=
 */
#define LAYOUT_LAYER_SYMBOLS                                                                  \
    KC_EXLM,   KC_AT, KC_HASH,  KC_DLR, KC_PERC,    KC_CIRC, KC_AMPR, KC_ASTR, KC_TILD,  KC_GRV, \
    KC_LABK, KC_LCBR, KC_LBRC, KC_LPRN, KC_MINS,     KC_EQL, KC_RPRN, KC_RBRC, KC_RCBR, KC_RABK, \
    KC_BSLS, KC_PIPE, KC_UNDS, KC_PLUS,  KC_GRV,    KC_TILD, KC_SLSH, KC_COLN, KC_SCLN, KC_QUES, \
                               _______, _______,    _______, XXXXXXX

/**
 * \brief Add Home Row mod to a layout.
 *
 * Expects a 10-key per row layout.  Adds support for GACS (Gui, Alt, Ctl, Shift)
 * home row.  The layout passed in parameter must contain at least 20 keycodes.
 *
 * This is meant to be used with `LAYER_ALPHAS_QWERTY` defined above, eg.:
 *
 *     HOME_ROW_MOD_GACS(LAYER_ALPHAS_QWERTY)
 */
#define _HOME_ROW_MOD_GACS(                                            \
    L00, L01, L02, L03, L04, R05, R06, R07, R08, R09,                  \
    L10, L11, L12, L13, L14, R15, R16, R17, R18, R19,                  \
    ...)                                                               \
             L00,         L01,         L02,         L03,         L04,  \
             R05,         R06,         R07,         R08,         R09,  \
      LGUI_T(L10), LALT_T(L11), LCTL_T(L12), LSFT_T(L13),        L14,  \
             R15,  RSFT_T(R16), RCTL_T(R17), RALT_T(R18), RGUI_T(R19), \
      __VA_ARGS__
#define HOME_ROW_MOD_GACS(...) _HOME_ROW_MOD_GACS(__VA_ARGS__)

/**
 * \brief Add pointer layer keys to a layout.
 *
 * Expects a 10-key per row layout.  The layout passed in parameter must contain
 * at least 30 keycodes.
 *
 * This is meant to be used with `LAYER_ALPHAS_QWERTY` defined above, eg.:
 *
 *     POINTER_MOD(LAYER_ALPHAS_QWERTY)
 */
#define _POINTER_MOD(                                                  \
    L00, L01, L02, L03, L04, R05, R06, R07, R08, R09,                  \
    L10, L11, L12, L13, L14, R15, R16, R17, R18, R19,                  \
    L20, L21, L22, L23, L24, R25, R26, R27, R28, R29,                  \
    ...)                                                               \
             L00,         L01,         L02,         L03,         L04,  \
             R05,         R06,         R07,         R08,         R09,  \
             L10,         L11,         L12,         L13,         L14,  \
             R15,         R16,         R17,         R18,         R19,  \
      _L_PTR(L20),        L21,         L22,         L23,         L24,  \
             R25,         R26,         R27,         R28,  _L_PTR(R29), \
      __VA_ARGS__
#define POINTER_MOD(...) _POINTER_MOD(__VA_ARGS__)

#define LAYOUT_wrapper(...) LAYOUT_split_3x5_2(__VA_ARGS__)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [LAYER_BASE] = LAYOUT_wrapper(LAYOUT_LAYER_BASE),
  [LAYER_NUMERAL] = LAYOUT_wrapper(LAYOUT_LAYER_NUMERAL),
  [LAYER_SYMBOLS] = LAYOUT_wrapper(LAYOUT_LAYER_SYMBOLS),
  [LAYER_NAVIGATION] = LAYOUT_wrapper(LAYOUT_LAYER_NAVIGATION),
  [LAYER_POINTER] = LAYOUT_wrapper(LAYOUT_LAYER_POINTER),
};
// clang-format on

#ifdef POINTING_DEVICE_ENABLE
#    ifdef DILEMMA_AUTO_POINTER_LAYER_TRIGGER_ENABLE
report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    if (abs(mouse_report.x) > DILEMMA_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD || abs(mouse_report.y) > DILEMMA_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD) {
        if (auto_pointer_layer_timer == 0) {
            layer_on(LAYER_POINTER);
        }
        auto_pointer_layer_timer = timer_read();
    }
    return mouse_report;
}

void matrix_scan_user(void) {
    if (auto_pointer_layer_timer != 0 && TIMER_DIFF_16(timer_read(), auto_pointer_layer_timer) >= DILEMMA_AUTO_POINTER_LAYER_TRIGGER_TIMEOUT_MS) {
        auto_pointer_layer_timer = 0;
        layer_off(LAYER_POINTER);
    }
}
#    endif // DILEMMA_AUTO_POINTER_LAYER_TRIGGER_ENABLE

#endif     // POINTING_DEVICE_ENABLE

// Tri-layer: NUM + SYM = NAV (works with OSL)
layer_state_t layer_state_set_user(layer_state_t state) {
    // If both NUM and SYM are active, also activate NAV
    if (layer_state_cmp(state, LAYER_NUMERAL) && layer_state_cmp(state, LAYER_SYMBOLS)) {
        state = state | (1UL << LAYER_NAVIGATION);
    }
#ifdef POINTING_DEVICE_ENABLE
#    ifdef DILEMMA_AUTO_SNIPING_ON_LAYER
    dilemma_set_pointer_sniping_enabled(layer_state_cmp(state, DILEMMA_AUTO_SNIPING_ON_LAYER));
#    endif
#endif
    return state;
}
