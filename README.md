# Custom Bootloader for Tiva C

This repo contains a custom bootloader for TM4C123GH6PM. The project is built using CCS and GNU ARM tool-chain.

## Description

command-based bootloader with the following commands:

1 - Erase Flash\
2 - Flash Request\
3 - Jump To Application

The reserved space for the bootloader is 24 KBs. The bootloader currently only accepts a raw binary application. Code execution happens
from RAM. Data is verified through a simple CRC algorithm.

## Plan

0 - Improvements to code design.\
1 - Support the S-RECORD and Intel Hex formats.\
2 - Support AES encryption.\
3 - Support having multiple applications.\
4 - Be locked with a password.\
5 - Support communication through CAN (Controller Area Network).

This was an experimental project, but I hope to continue improving it :).
