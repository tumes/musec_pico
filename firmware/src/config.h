/*
 * Controller Config
 * WHowe <github.com/whowechina>
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t rgb_hsv; // 0: RGB, 1: HSV
    uint8_t val[3]; // RGB or HSV
} rgb_hsv_t;

typedef struct __attribute__((packed)) {
    struct {
        uint16_t min;
        uint16_t max;
        uint8_t invert:1;
        uint8_t reserved:7;
    } lever;
    struct {
        rgb_hsv_t colors[12];
        uint8_t level;
        uint8_t reserved[15];
    } light;
    struct {
        uint8_t volume;
        uint8_t reserved[3];
    } sound;
    struct {
        uint8_t roi;
        uint8_t reserved[7];
    } tof;
    struct {
        uint8_t joy : 4;
        uint8_t nkro : 4;
    } hid;
    struct {
        uint8_t mode : 4;
        uint8_t virtual_aic : 4;
    } aime;
} musec_cfg_t;

typedef struct {
    uint16_t fps[2];
    bool key_stuck;
} musec_runtime_t;

extern musec_cfg_t *musec_cfg;
extern musec_runtime_t musec_runtime;

void config_init();
void config_changed(); // Notify the config has changed
void config_factory_reset(); // Reset the config to factory default

#endif
