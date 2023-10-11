#ifndef BOOTLOADER_CONFIG_H
#define BOOTLOADER_CONFIG_H

/* Image Formats */
#define SRECORD_IMAGE 0

/* Communication Interfaces */
#define USE_UART 

/* Booting modes */
#define AUTO_BOOTING 0
#define COMMAND_BOOTING 1

#define BOOTING_MODE COMMAND_BOOTING
#define IMAGE_FORMAT SRECORD_IMAGE

#endif