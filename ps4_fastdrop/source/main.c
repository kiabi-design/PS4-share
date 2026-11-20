#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <orbis/libkernel.h>
#include <orbis/Sysmodule.h>
#include <orbis/Pad.h>
#include <orbis/UserService.h>
#include <orbis/VideoOut.h>

/*
 * FastDrop PS4 — UI Skeleton (Dark + Green theme)
 * Interactive layout: Send, Receive, History, Settings/Folders, Quick Select.
 * Stub mode: UI responds to controls and shows state without real file/PKG actions yet.
 */

typedef enum {
    TAB_SEND = 0,
    TAB_RECEIVE,
    TAB_HISTORY,
    TAB_SETTINGS
} AppTab;

typedef enum {
    PKG_MODE_DIRECT_INSTALL = 0,
    PKG_MODE_SAVE_FILE
} PkgMode;

typedef struct {
    AppTab active_tab;
    int selected_index;
    PkgMode pkg_mode;

    // Stub settings & folders (display only for now)
    bool auto_sort_extensions;
    bool high_speed_buffer_256k;
    int target_ip_index;
    int folder_pkg_index;
    int folder_prx_index;
    int folder_general_index;

    // Stub status & history log
    char status_line[256];
    char history[4][128];
    int history_count;
} FastDropUiState;

static const char *QUICK_TARGETS[3] = {
    "192.168.10.1 (Cable / Static)",
    "192.168.1.100 (Phone 5GHz)",
    "172.20.10.1 (Hotspot)"
};

static const char *PKG_FOLDERS[2] = {
    "/data/FastDrop/pkg/ [stub]",
    "/mnt/usb0/FastDrop/pkg/ [stub]"
};

static const char *PRX_FOLDERS[2] = {
    "/data/GoldHEN/plugins/ [stub]",
    "/mnt/usb0/GoldHEN/plugins/ [stub]"
};

static const char *GENERAL_FOLDERS[2] = {
    "/data/FastDrop/files/ [stub]",
    "/mnt/usb0/FastDrop/files/ [stub]"
};

static void push_history(FastDropUiState *st, const char *entry) {
    if (st->history_count < 4) {
        snprintf(st->history[st->history_count], sizeof(st->history[0]), "%s", entry);
        st->history_count++;
    } else {
        for (int i = 0; i < 3; i++) {
            snprintf(st->history[i], sizeof(st->history[i]), "%s", st->history[i + 1]);
        }
        snprintf(st->history[3], sizeof(st->history[3]), "%s", entry);
    }
}

static void init_ui_state(FastDropUiState *st) {
    memset(st, 0, sizeof(*st));
    st->active_tab = TAB_SEND;
    st->selected_index = 0;
    st->pkg_mode = PKG_MODE_DIRECT_INSTALL;
    st->auto_sort_extensions = true;
    st->high_speed_buffer_256k = true;
    st->target_ip_index = 0;
    st->folder_pkg_index = 0;
    st->folder_prx_index = 0;
    st->folder_general_index = 0;

    snprintf(st->status_line, sizeof(st->status_line),
             "Theme: Dark/Emerald | Stub UI Ready (No real transfer/install yet)");

    push_history(st, "[STUB] Sent sample_video.mp4 -> 192.168.10.1 (24.5 MB/s mock)");
    push_history(st, "[STUB] Received theme_patch.prx -> /data/GoldHEN/plugins/");
    push_history(st, "[STUB] PKG queued: game_update.pkg (Direct Install mock)");
}

/*
 * Render hook: replace or wire `render_fastdrop_screen` into your OpenOrbis
 * framebuffer / ImGui / Cairo / Gnm drawing loop.
 *
 * Palette (Dark + Harmonious Green):
 *   Background primary : #07090B
 *   Card / Panel bg    : #0C1410
 *   Border / Outline   : #1B3B2B
 *   Text primary       : #7BFF9C
 *   Text secondary     : #48A868
 *   Accent active      : #2ECC71
 */
static void render_fastdrop_screen(const FastDropUiState *st) {
    const char *tab_names[] = {
        "SEND (OTPRavit)",
        "RECEIVE (PRINYAT)",
        "HISTORY",
        "SETTINGS & FOLDERS"
    };

    printf("\n================ FASTDROP PS4 [UI SKELETON] ================\n");
    printf("Nav: [L1/R1] Tab: %s\n", tab_names[st->active_tab]);
    printf("Target: %s | Buffer: %s | AutoSort(.pkg/.prx): %s\n",
           QUICK_TARGETS[st->target_ip_index],
           st->high_speed_buffer_256k ? "256 KB [ON]" : "64 KB [OFF]",
           st->auto_sort_extensions ? "ON" : "OFF");
    printf("------------------------------------------------------------\n");

    if (st->active_tab == TAB_SEND) {
        printf("  > [BUTTON] Send File / Media (Stub)\n");
        printf("  > [BUTTON] Send .PKG -> Mode: %s (Stub)\n",
               st->pkg_mode == PKG_MODE_DIRECT_INSTALL ? "Install Now" : "Save as .PKG File");
        printf("  > [BUTTON] Switch Quick Target IP (Stub)\n");
    } else if (st->active_tab == TAB_RECEIVE) {
        printf("  > [BUTTON] Start / Stop Receive Listener (Stub)\n");
        printf("  > [INFO]   Incoming .pkg -> %s\n", PKG_FOLDERS[st->folder_pkg_index]);
        printf("  > [INFO]   Incoming .prx -> %s\n", PRX_FOLDERS[st->folder_prx_index]);
        printf("  > [INFO]   Other files  -> %s\n", GENERAL_FOLDERS[st->folder_general_index]);
    } else if (st->active_tab == TAB_HISTORY) {
        for (int i = 0; i < st->history_count; i++) {
            printf("  %d. %s\n", i + 1, st->history[i]);
        }
        printf("  > [BUTTON] Clear History (Stub)\n");
    } else if (st->active_tab == TAB_SETTINGS) {
        printf("  > [FOLDER] .PKG folder : %s\n", PKG_FOLDERS[st->folder_pkg_index]);
        printf("  > [FOLDER] .PRX folder : %s\n", PRX_FOLDERS[st->folder_prx_index]);
        printf("  > [FOLDER] Save folder : %s\n", GENERAL_FOLDERS[st->folder_general_index]);
        printf("  > [TOGGLE] Auto-sort .pkg / .prx: %s\n", st->auto_sort_extensions ? "Enabled" : "Disabled");
        printf("  > [TOGGLE] 256 KB Stream Buffer: %s\n", st->high_speed_buffer_256k ? "Enabled" : "Disabled");
    }

    printf("------------------------------------------------------------\n");
    printf("Status: %s\n", st->status_line);
    printf("============================================================\n");
}

static void handle_action(FastDropUiState *st) {
    if (st->active_tab == TAB_SEND) {
        if (st->selected_index == 0) {
            snprintf(st->status_line, sizeof(st->status_line),
                     "[STUB] Clicked SEND: ready to pick file -> %s",
                     QUICK_TARGETS[st->target_ip_index]);
            push_history(st, "[STUB] Send button pressed (no network transfer yet)");
        } else if (st->selected_index == 1) {
            st->pkg_mode = (st->pkg_mode == PKG_MODE_DIRECT_INSTALL)
                           ? PKG_MODE_SAVE_FILE
                           : PKG_MODE_DIRECT_INSTALL;
            snprintf(st->status_line, sizeof(st->status_line),
                     "[STUB] PKG mode switched to: %s",
                     st->pkg_mode == PKG_MODE_DIRECT_INSTALL ? "Install Now" : "Save as .PKG File");
        } else {
            st->target_ip_index = (st->target_ip_index + 1) % 3;
            snprintf(st->status_line, sizeof(st->status_line),
                     "[STUB] Target selected: %s", QUICK_TARGETS[st->target_ip_index]);
        }
    } else if (st->active_tab == TAB_RECEIVE) {
        snprintf(st->status_line, sizeof(st->status_line),
                 "[STUB] Clicked RECEIVE: listener UI toggled (folders displayed only)");
        push_history(st, "[STUB] Receive button pressed (stub mode)");
    } else if (st->active_tab == TAB_HISTORY) {
        st->history_count = 0;
        snprintf(st->status_line, sizeof(st->status_line),
                 "[STUB] History cleared in UI state");
    } else if (st->active_tab == TAB_SETTINGS) {
        if (st->selected_index == 0) {
            st->folder_pkg_index = (st->folder_pkg_index + 1) % 2;
        } else if (st->selected_index == 1) {
            st->folder_prx_index = (st->folder_prx_index + 1) % 2;
        } else if (st->selected_index == 2) {
            st->folder_general_index = (st->folder_general_index + 1) % 2;
        } else if (st->selected_index == 3) {
            st->auto_sort_extensions = !st->auto_sort_extensions;
        } else {
            st->high_speed_buffer_256k = !st->high_speed_buffer_256k;
        }
        snprintf(st->status_line, sizeof(st->status_line),
                 "[STUB] Setting updated on screen (not applied to PS4 filesystem yet)");
    }
}

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    sceSysmoduleLoadModule(ORBIS_SYSMODULE_USER_SERVICE);
    sceSysmoduleLoadModule(ORBIS_SYSMODULE_PAD);

    sceUserServiceInitialize(NULL);
    scePadInit();

    OrbisUserServiceUserId user_id = -1;
    sceUserServiceGetInitialUser(&user_id);
    int pad_handle = scePadOpen(user_id, 0, 0, NULL);

    FastDropUiState ui;
    init_ui_state(&ui);

    uint32_t prev_buttons = 0;
    render_fastdrop_screen(&ui);

    while (1) {
        if (pad_handle >= 0) {
            OrbisPadData pad_data;
            memset(&pad_data, 0, sizeof(pad_data));
            if (scePadReadState(pad_handle, &pad_data) == 0 && pad_data.connected) {
                uint32_t buttons = pad_data.buttons;
                uint32_t pressed = buttons & ~prev_buttons;
                prev_buttons = buttons;

                bool changed = false;

                if (pressed & ORBIS_PAD_BUTTON_R1) {
                    ui.active_tab = (AppTab)((ui.active_tab + 1) % 4);
                    ui.selected_index = 0;
                    changed = true;
                }
                if (pressed & ORBIS_PAD_BUTTON_L1) {
                    ui.active_tab = (AppTab)((ui.active_tab + 3) % 4);
                    ui.selected_index = 0;
                    changed = true;
                }
                if (pressed & ORBIS_PAD_BUTTON_DOWN) {
                    ui.selected_index = (ui.selected_index + 1) % 5;
                    changed = true;
                }
                if (pressed & ORBIS_PAD_BUTTON_UP) {
                    ui.selected_index = (ui.selected_index + 4) % 5;
                    changed = true;
                }
                if (pressed & ORBIS_PAD_BUTTON_CROSS) {
                    handle_action(&ui);
                    changed = true;
                }

                if (changed) {
                    render_fastdrop_screen(&ui);
                }
            }
        }
        sceKernelUsleep(16666);
    }

    return 0;
}
