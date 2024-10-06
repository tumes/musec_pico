#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>

#include "pico/stdio.h"
#include "pico/stdlib.h"

#include "config.h"
#include "save.h"
#include "cli.h"

#include "tmag5273.h"

#include "usb_descriptors.h"

#define SENSE_LIMIT_MAX 9
#define SENSE_LIMIT_MIN -9

static void disp_light()
{
    printf("[Light]\n");
    printf("  Level: %d.\n", musec_cfg->light.level);
}

static void disp_hid()
{
    printf("[HID]\n");
    printf("  Joy: %s, NKRO: %s.\n", 
           musec_cfg->hid.joy ? "on" : "off",
           musec_cfg->hid.nkro ? "on" : "off" );
}

static void disp_spin()
{
    printf("[Spin]\n");
    printf("  Units per turn: %d.\n", musec_cfg->spin.units_per_turn);
}

void handle_display(int argc, char *argv[])
{
    const char *usage = "Usage: display [light|spin|hid]\n";
    if (argc > 1) {
        printf(usage);
        return;
    }

    if (argc == 0) {
        disp_light();
        disp_spin();
        disp_hid();
        return;
    }

    const char *choices[] = {"light", "spin", "hid" };
    switch (cli_match_prefix(choices, count_of(choices), argv[0])) {
        case 0:
            disp_light();
            break;
        case 1:
            disp_spin();
            break;
        case 2:
            disp_hid();
            break;
        default:
            printf(usage);
            break;
    }
}

static int fps[2];
void fps_count(int core)
{
    static uint32_t last[2] = {0};
    static int counter[2] = {0};

    counter[core]++;

    uint32_t now = time_us_32();
    if (now - last[core] < 1000000) {
        return;
    }
    last[core] = now;
    fps[core] = counter[core];
    counter[core] = 0;
}

static void handle_level(int argc, char *argv[])
{
    const char *usage = "Usage: level <0..255>\n";
    if (argc != 1) {
        printf(usage);
        return;
    }

    int level = cli_extract_non_neg_int(argv[0], 0);
    if ((level < 0) || (level > 255)) {
        printf(usage);
        return;
    }

    musec_cfg->light.level = level;
    config_changed();
    disp_light();
}

static void handle_spin(int argc, char *argv[])
{
    const char *usage = "Usage: spin <units_per_turn>\n"
                        "  units_per_turn: 20..255\n";
    if (argc != 1) {
        printf(usage);
        return;
    }

    int units = cli_extract_non_neg_int(argv[0], 0);
    if ((units < 20) || (units > 255)) {
        printf(usage);
        return;
    }

    musec_cfg->spin.units_per_turn = units;
    config_changed();
    disp_spin();
}

static void handle_hid(int argc, char *argv[])
{
    const char *usage = "Usage: hid <joy|nkro|both>\n";
    if (argc != 1) {
        printf(usage);
        return;
    }

    const char *choices[] = {"joy", "nkro", "both"};
    int match = cli_match_prefix(choices, 3, argv[0]);
    if (match < 0) {
        printf(usage);
        return;
    }

    musec_cfg->hid.joy = ((match == 0) || (match == 2)) ? 1 : 0;
    musec_cfg->hid.nkro = ((match == 1) || (match == 2)) ? 1 : 0;
    config_changed();
    disp_hid();
}

static void handle_save()
{
    save_request(true);
}

static void handle_factory_reset()
{
    config_factory_reset();
    printf("Factory reset done.\n");
}

void commands_init()
{
    cli_register("display", handle_display, "Display all config.");
    cli_register("level", handle_level, "Set LED brightness level.");
    cli_register("spin", handle_spin, "Set spin rate.");
    cli_register("hid", handle_hid, "Set HID mode.");
    cli_register("save", handle_save, "Save config to flash.");
    cli_register("factory", handle_factory_reset, "Reset everything to default.");
}
