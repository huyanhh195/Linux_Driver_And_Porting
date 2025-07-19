# Character Device Driver for Controlling Raspberry Pi 4 LED

## Introduction

This is a simple Linux kernel module implementing a character device driver to control an LED connected to the Raspberry Pi 4 via GPIO. The module enables turning the LED on, off, or blinking by writing commands from user space to the device file `/dev/gpio`.

This module supports building both directly on the x86_64 PCs and Raspberry Pi 4 using the Yocto Project.

## Versions

| Component            | Version                           |
|----------------------|-----------------------------------|
| Yocto                | Kirkstone                         |
| Host OS (Ubuntu)     | 22.04 LTS (Jammy Jellyfish)       |
| Raspberry Pi 4 Kernel| 5.15.x                            |
| Host Kernel          | 6.8.0-64-generic                  |

---

## Build Instructions
### Building on PC
- To build and load the kernel module on your PC, run the following commands:
```bash
make
sudo insmod mgpio.ko
```
- To unload the module when no longer needed:
```bash
sudo rmmod mgpio
```
### Building on Raspberry Pi 4
To build the module for Raspberry Pi 4, please use the following Yocto meta-layer repository:
https://github.com/huyanhh195/Yocto
Clone this repository and include it in your Yocto build environment to ensure proper integration and compatibility.