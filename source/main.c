#include "ps4.h"

#define USB0_PATH "/mnt/usb0/"
#define USB1_PATH "/mnt/usb1/"
#define CONFIG_FILE "config.ini"
#define DIR_TEST ".dirtest"

char titleId[20];
int method;
int usb;
char ini_file_path[256];
char inprogress_file_path[256];
char complete_file_path[256];

int read_config() {
    if (!file_exists(ini_file_path)) {
        return -1;  // File doesn't exist
    }

    int cfile = open(ini_file_path, O_RDONLY, 0);
    if (cfile < 0) {
        return -1;  // Unable to open file
    }

    char *idata = read_string(cfile);
    close(cfile);

    if (strlen(idata) == 0) {
        free(idata);
        return -1;  // Empty file
    }

    char *line = strtok(idata, "\n");
    while (line != NULL) {
        while (line != NULL) {
            if (strstr(line, "titleId=") != NULL) {
                sscanf(line, "titleId=%19s", titleId);  // Read titleId
            } else if (strstr(line, "method=") != NULL) {
                sscanf(line, "method=%d", &method);  // Read method
            }
        line = strtok(NULL, "\n");
        }
    }
    free(idata);
    return 0;  // Success
}

int setup_usb() {
    int usbdir = open(USB0_PATH DIR_TEST, O_WRONLY | O_CREAT | O_TRUNC, 0777);
    if (usbdir != -1) {
        close(usbdir);
        unlink(USB0_PATH DIR_TEST);
        snprintf(ini_file_path, sizeof(ini_file_path), "%s%s", USB0_PATH, CONFIG_FILE);
        usb = 0;
        return 0;
    }

    usbdir = open(USB1_PATH DIR_TEST, O_WRONLY | O_CREAT | O_TRUNC, 0777);
    if (usbdir != -1) {
        close(usbdir);
        unlink(USB1_PATH DIR_TEST);
        snprintf(ini_file_path, sizeof(ini_file_path), "%s%s", USB1_PATH, CONFIG_FILE);
        usb = 1;
        return 0;
    }

    return -1;  // USB not found
}

void create_inprogress_file() {
    snprintf(inprogress_file_path, sizeof(inprogress_file_path), "/mnt/usb%i/%s.inprogress", usb, titleId);
    int fd = open(inprogress_file_path, O_WRONLY | O_CREAT | O_TRUNC, 0777);
    if (fd >= 0) {
        close(fd);
    }
}

void remove_inprogress_file() {
    unlink(inprogress_file_path);
}

void create_complete_file() {
    snprintf(complete_file_path, sizeof(complete_file_path), "/mnt/usb%i/%s.complete", usb, titleId);
    int fd = open(complete_file_path, O_WRONLY | O_CREAT | O_TRUNC, 0777);
    if (fd >= 0) {
        close(fd);
    }
}

void backup_game() {
    printf_notification("Backing up Base Game...");
    char src_path[256], dest_path[256], dir_path[256];

    snprintf(src_path, sizeof(src_path), "/user/app/%s/app.pkg", titleId);
    snprintf(dest_path, sizeof(dest_path), "/mnt/usb%i/%s-app/app.pkg", usb, titleId);
    snprintf(dir_path, sizeof(dir_path), "/mnt/usb%i/%s-app/", usb, titleId);

    if (file_exists(src_path)) {
        mkdir(dir_path, 0777);
        copy_file(src_path, dest_path);
        printf_notification("Base Game backup completed.");
    } else {
        printf_notification("app.pkg not found. Skipping...");
    }
}

void backup_update() {
    printf_notification("Backing up Update...");
    char src_path[256], dest_path[256], dir_path[256];

    snprintf(src_path, sizeof(src_path), "/user/patch/%s/patch.pkg", titleId);
    snprintf(dest_path, sizeof(dest_path), "/mnt/usb%i/%s-patch/patch.pkg", usb, titleId);
    snprintf(dir_path, sizeof(dir_path), "/mnt/usb%i/%s-patch/", usb, titleId);

    if (file_exists(src_path)) {
        mkdir(dir_path, 0777);
        copy_file(src_path, dest_path);
        printf_notification("Update backup completed.");
    } else {
        printf_notification("patch.pkg not found. Skipping...");
    }
}

void backup_dlcs() {
    printf_notification("Backing up DLCs...");
    char src_path[256], dest_path[256];

    snprintf(src_path, sizeof(src_path), "/user/addcont/%s", titleId);
    snprintf(dest_path, sizeof(dest_path), "/mnt/usb%i/%s-dlc", usb, titleId);

    if (dir_exists(src_path)) {
        copy_dir(src_path, dest_path);
        printf_notification("DLCs backup completed.");
    } else {
        printf_notification("DLCs not found. Skipping...");
    }
}

int _main(struct thread *td) {
    UNUSED(td);

    initKernel();
    initLibc();
    initPthread();
    jailbreak();
    initSysUtil();

    printf_notification("Running PKG-Backup\nBy harsha-0110");

    if (setup_usb() < 0) {
        printf_notification("USB not found!\nSkipping Backup...");
        return 0;
    }

    if (read_config() < 0) {
        printf_notification("Failed to read config.ini\n");
        return 0;
    }

    printf_notification("Title ID: %s\nMethod: %d", titleId, method);

    create_inprogress_file();  // Create the in-progress file

    switch (method) {
        case 1:  // Backup app.pkg
            backup_game();
            break;

        case 2:  // Backup patch.pkg
            backup_update();
            break;

        case 3:  // Backup DLCs
            backup_dlcs();
            break;

        case 4:  // Backup all
            backup_game();
            backup_update();
            backup_dlcs();
            break;

        default:
            printf_notification("Invalid method specified");
            break;
    }

    remove_inprogress_file();  // Remove the in-progress file
    create_complete_file();    // Create the complete file

    return 0;
}
