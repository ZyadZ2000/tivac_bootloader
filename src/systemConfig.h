#ifndef SYSTEM_CONFIG_H
#define SYSTEM_CONFIG_H

#define ENABLE 1
#define DISABLE 0
#define AES_ENCRYPTION 2

/* Image Formats */
#define SRECORD_IMAGE 0

/* Communication Interfaces */
#define UART_COMM ENABLE

/* Booting modes */
#define AUTO_MODE 0
#define COMMAND_MODE 1

/* Security */
#define BOOTLOADER_PASSWORD DISABLE
#define IMAGE_ENCRYPTION    DISABLE

#define BOOTING_MODE COMMAND_MODE
#define IMAGE_FORMAT SRECORD_IMAGE

#endif
