# Custom-USB-Device-Driver-for-Data-Acquisition

a custom USB device driver and embedded firmware for real-time MPU6050 sensor data acquisitionusing the STM32F407G-DISC board. Implemented USB Full-Speed bulk communication, custom USB descriptors,and Linux kernel APIs to enable reliable kernel-space to user-space data transfer and seamless embedded device-to-host communication.

A custom usb device driver for data acquisition from a device sending MPU6050 accelerometer sensor values of X Y Z axis. STM32F407G DISC board is used as a custom USB with 12Mbps FS device through which sensor values will be passed to user programme through usb driver.

Objective
TO design and implement a custom USB device driver in the Linux kernel for sensor data acquisition. The system uses an MPU6050 sensor connected to an STM32 microcontroller through I²C. The STM32 acquires the sensor data and transfers it to the Linux host through a custom USB interface. A custom Linux USB kernel driver is developed to identify the USB device, receive the sensor data, and make the acquired data available to a user-space application for further processing and analysis.


Key Objectives

1. Develop a custom Linux USB device driver using the Linux USB driver framework.
2. Interface the MPU6050 accelerometer with the STM32 microcontroller using I²C.
3. Transfer sensor data from MPU6050 → STM32 → USB → Linux kernel driver.
4. implement USB driver operations such as probe, disconnect, read, open, and release.
5. Identify the USB device using its VID/PID and bind it to the custom driver.
6. Provide a mechanism for user-space applications to read sensor data from the driver.
7. Understand and demonstrate the complete USB communication and Linux device-driver flow.

<img width="1204" height="503" alt="Screenshot from 2026-09-01 15-03-15" src="https://github.com/user-attachments/assets/f58a1e64-b85c-4ecc-9389-427421a4ed74" />











