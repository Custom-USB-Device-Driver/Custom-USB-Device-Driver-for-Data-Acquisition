# Custom-USB-Device-Driver-for-Data-Acquisition

*****Project Overview*****

a custom USB device driver and embedded firmware for real-time MPU6050 sensor data acquisitionusing the STM32F407G-DISC board. Implemented USB Full-Speed bulk communication, custom USB descriptors,and Linux kernel APIs to enable reliable kernel-space to user-space data transfer and seamless embedded device-to-host communication.

A custom usb device driver for data acquisition from a device sending MPU6050 accelerometer sensor values of X Y Z axis. STM32F407G DISC board is used as a custom USB with 12Mbps FS device through which sensor values will be passed to user programme through usb driver.

The MPU6050 is interfaced with the STM32 through the I²C protocol. The STM32 continuously reads accelerometer data from the MPU6050, processes the change in acceleration, and detects vibration based on a predefined threshold. The acquired data is transmitted from the STM32 to a Linux host through USB, where a custom Linux USB device driver is responsible for receiving and handling the USB data.

The project demonstrates the complete data acquisition path from a physical sensor to a Linux kernel driver and finally to user space.


****Key Objectives****

1. Develop a custom Linux USB device driver using the Linux USB driver framework.
2. Interface the MPU6050 accelerometer with the STM32 microcontroller using I²C.
3. Transfer sensor data from MPU6050 → STM32 → USB → Linux kernel driver.
4. implement USB driver operations such as probe, disconnect, read, open, and release.
5. Identify the USB device using its VID/PID and bind it to the custom driver.
6. Provide a mechanism for user-space applications to read sensor data for monitoring and further processing.
7. Understand and demonstrate the complete USB communication and Linux device-driver flow.


****System Architecture****

<img width="1204" height="503" alt="Screenshot from 2026-09-01 15-03-15" src="https://github.com/user-attachments/assets/f58a1e64-b85c-4ecc-9389-427421a4ed74" />

  
The system consists of four major layers: sensor layer, embedded processing layer, USB communication layer, and Linux kernel/user-space layer.

**a.** **MPU6050 Sensor**

The MPU6050 is the sensing component of the system. It provides three-axis accelerometer data along the X, Y, and Z axes.
The sensor is interfaced with the STM32 microcontroller using the I²C communication protocol.

The STM32 reads six bytes from the MPU6050 accelerometer registers:
2 bytes for X-axis acceleration
2 bytes for Y-axis acceleration
2 bytes for Z-axis acceleration
The two bytes corresponding to each axis are combined to form a signed 16-bit acceleration value.

**b.** **STM32 Microcontroller**

The STM32 acts as the embedded controller and USB device.
Its main responsibilities are:
                                      1.Initialize the I²C peripheral.
                                      2.Initialize the MPU6050.
                                      3.Read accelerometer data.
                                      4.Process the acquired sensor data.
                                      5.Prepare the data for USB transmission.
                                      6.Transfer the data to the Linux host through USB.

The MPU6050 initialization is performed by writing to its power-management register : MPU6050_Init(&hi2c1);

Sensor data is acquired using:  MPU6050_Read_Accel (&hi2c1 ,&accel_x, &accel_y,  &accel_z);
Thus, the STM32  device acts as the bridge between the physical sensor and the Linux system.
                                         
**c.** **USB Communication**

USB provides the communication link between the STM32 device and the Linux host.
The STM32 operates as the USB device, while the Linux computer acts as the USB host.
The custom Linux driver identifies the target STM32 USB device using its Vendor ID (VID) and Product ID (PID).

#define USB_VENDOR_ID   0x0483
#define USB_PRODUCT_ID  0x5740

The driver therefore matches the USB device:            VID : 0x0483
                                                        PID : 0x5740

When the STM32 USB device is connected, the Linux USB subsystem detects and enumerates the device. The USB subsystem then compares the device's VID/PID with the IDs present in the custom driver's USB device table i.e.

static struct usb_device_id mpu_table[] =
                                                {
                                                    {
                                                        USB_DEVICE(USB_VENDOR_ID, USB_PRODUCT_ID)
                                                    },
                                                    {}
                                                };

**d.** **Custom Linux USB Driver**

The main component is the custom USB device driver.the driver is implemented using the Linux USB driver framework.
Its main responsibilities are:
                                    1.Detect the target USB device.
                                    2.Handle device connection.
                                    3.Examine the USB interface.
                                    4.Locate the Bulk IN endpoint.
                                    5.Allocate a kernel buffer.
                                    6.Receive data from the USB device.
                                    7.Decode the received sensor bytes.
                                    8.Handle device disconnection

The probe() function is called when Linux finds a USB device matching the driver's ID table.
The disconnect() function is called when the USB device is removed.

**d1.** **USB Driver Implementation**

The driver uses the Linux USB framework:

                 static struct usb_driver mpu_driver =      {
                                                               .name       = "mpu6050_usb",
                                                                .probe      = mpu_probe,
                                                                .disconnect = mpu_disconnect,
                                                                .id_table   = mpu_table,
                                                            };

The driver is registered using:  module_usb_driver(mpu_driver);
This allows the Linux USB subsystem to automatically associate the driver with a matching USB device.
The driver uses a USB device ID table:
                                            static struct usb_device_id mpu_table[] =   {
                                                                                            {
                                                                                                USB_DEVICE(USB_VENDOR_ID, USB_PRODUCT_ID)
                                                                                            },
                                                                                            {}
                                                                                        };

**d2.** **Device-Specific Structure**

The driver maintains device-specific information using:

                                                  struct mpu6050
                                                                        {
                                                                            struct usb_device *udev;
                                                                            struct usb_interface *interface;
                                                                        
                                                                            unsigned char *bulk_in_buffer;
                                                                            size_t bulk_in_size;
                                                                        
                                                                            __u8 bulk_in_endpointAddr;
                                                                        };

This structure stores:                                            USB device pointer.
                                                                  USB interface pointer.
                                                                  Bulk IN receive buffer.
                                                                  Buffer size.
                                                                  Bulk IN endpoint address.



**e.** **USB Bulk IN Endpoint**

The endpoint is used for receiving data from the STM32.The driver searches the selected USB interface for a Bulk IN endpoint.
if (usb_endpoint_is_bulk_in(endpoint))
                                               {
                                                  dev->bulk_in_endpointAddr = endpoint->bEndpointAddress;
                                              }

The driver then allocates a kernel buffer :   dev->bulk_in_buffer = kmalloc(dev->bulk_in_size, GFP_KERNEL);
The USB data is received using:  usb_bulk_msg();

**f.** **Sensor Data Decoding**

The custom driver expects the sensor information in a six-byte format.The driver reconstructs each 16-bit value:

                            accel_x = (int16_t)((buffer[0] << 8) | buffer[1]);
                            accel_y = (int16_t)((buffer[2] << 8) | buffer[3]);
                            accel_z = (int16_t)((buffer[4] << 8) | buffer[5]);                            
result printed using : pr_info("Accel X=%d Y=%d Z=%d\n",accel_x,accel_y,accel_z);

**g.** **USB Data Reception**

The driver receives data using:  usb_bulk_msg()                 
The important part of the implementation is: ret = usb_bulk_msg( dev->udev,usb_rcvbulkpipe(dev->udev,dev->bulk_in_endpointAddr),dev->bulk_in_buffer,dev->bulk_in_size,&actual_length,5000 );

**h.** **MPU6050 Interface**

The MPU6050 is connected to the STM32 using I²C.
Initialization The MPU6050 is taken out of sleep mode by writing 0x00 to the power-management register: 
                                                                  uint8_t data = 0x00;
                                                                  HAL_I2C_Mem_Write( hi2c, MPU6050_ADDR,MPU6050_PWR1,1,&data,1,HAL_MAX_DELAY);

**i.** **Accelerometer Acquisition**

Six bytes are read from the accelerometer registers:
                                                            uint8_t buffer[6];
                                                            HAL_I2C_Mem_Read(hi2c, MPU6050_ADDR,ACCEL_XOUT_H,1,buffer,6,HAL_MAX_DELAY);
                                                            
                                                            The six bytes represent:
                                                                                    AX High + AX Low
                                                                                    AY High + AY Low
                                                                                    AZ High + AZ Low

**j.** **Vibration Detection**

The STM32 firmware also calculates the change in acceleration between consecutive samples.

vibration = abs(accel_x - prev_x) + abs(accel_y - prev_y) + abs(accel_z - prev_z);


**l.** **Driver Probe Function**

The probe() function is the entry point when Linux finds a USB device matching the driver's ID table.

The driver performs the following operations:

                                                probe()
                                                            Allocate device structure
                                                            Get USB device reference
                                                            Store interface
                                                            Get current interface descriptor
                                                            Check interface number
                                                            Search for Bulk IN endpoint
                                                            Allocate receive buffer
                                                            Store driver data
                                                            Start USB data acquisition


**m.** **Disconnect Handling**

When the USB device is removed, Linux calls : mpu_disconnect()

                                           The driver:
                                                          Retrieves the stored device structure.
                                                          Clears interface data.
                                                          Frees the USB receive buffer.
                                                          Releases the USB device reference.
                                                          Frees the device structure.






**h.** **User-Space**

The final layer is the user-space .The purpose of the user-space layer is to access, display, or further process the sensor information received through the Linux driver.


****Flow Chart****

<img width="1101" height="861" alt="image" src="https://github.com/user-attachments/assets/7fe985fe-336a-4c9c-97ed-f84b3175252a" />

****Linux Driver Technologies Used****
                                            Technology	                               Purpose
                                            linux/module.h	                            Kernel module support
                                            linux/usb.h	                                Linux USB driver framework
                                            linux/slab.h	                              Kernel memory allocation
                                            linux/fs.h	                                File-system/device interfaces
                                            linux/uaccess.h                            	User/kernel data transfer support
                                            usb_device_id	                              USB device matching
                                            usb_bulk_msg()                             	Bulk USB transfer
                                            usb_rcvbulkpipe()	                          Configure USB receive pipe
                                            kzalloc()	                                  Allocate zero-initialized memory
                                            kmalloc()	                                  Allocate kernel buffer
                                            usb_get_dev()	                              Obtain USB device reference
                                            usb_put_dev()                              	Release USB device reference


****STM32 Technologies Used****
                                            Technology                  	Purpose
                                            STM32	Embedded                controller
                                            STM32 HAL	                    Hardware abstraction
                                            I²C	MPU6050                   communication
                                            USB	STM32                    ↔ Linux communication
                                            MPU6050                      	Motion/acceleration sensing
                                            C                            	Firmware implementation


Linux Debugging Commands

                                              Check whether the device is detected:     lsusb 
                                              Monitor kernel messages:                  dmesg -w
                                              
                                              Check loaded driver:                      lsmod
                                              
                                              Check driver information:                 modinfo mpu6050_usb.ko
                                              
                                              Load the module:                          sudo insmod mpu6050_usb.ko
                                              
                                              Remove the module:                        sudo rmmod mpu6050_usb

****Challenges Faced****

1. USB Driver Binding
The custom driver must be associated with the correct USB device.
Solution: VID/PID matching is implemented using the Linux USB device ID table.

3. Interface Selection
The device exposes multiple USB interfaces, so the driver checks the interface number before continuing.
if (iface_desc->desc.bInterfaceNumber != 1)
{
    return -ENODEV;
}

3. Bulk IN Endpoint Detection
Instead of assuming a fixed endpoint address, the driver examines the interface's endpoint descriptors and searches for a Bulk IN endpoint.

4. Kernel Memory Management
The driver dynamically allocates memory for:
                                              Device structure.
                                              USB receive buffer.
The allocated resources are released during error handling and device disconnection.

5. USB Data Reception
The driver uses usb_bulk_msg() to synchronously receive data from the USB device.

6. Sensor Data Decoding
The received byte stream is converted into signed 16-bit X, Y and Z acceleration values.



****Project Summary****

This project demonstrates the integration of embedded sensor acquisition and Linux kernel USB driver development.
The MPU6050 provides motion data to the STM32 through I²C. The STM32 acts as the USB device and transfers the acquired information to a Linux host. The custom Linux USB driver identifies the device using VID/PID, locates the Bulk IN endpoint, allocates a receive buffer, receives USB data using usb_bulk_msg(), and reconstructs the sensor values from the received byte stream.


 ****Project Information****
                                Project: Custom USB Device Driver for MPU6050 Sensor Data Acquisition
                                Domain: Linux Device Drivers / Embedded Systems
                                Microcontroller: STM32
                                Sensor: MPU6050
                                Sensor Communication: I²C
                                Host Communication: USB
                                Host Operating System: Linux
                                Driver Type: Custom Linux USB Kernel Driver
                                Programming Languages: C / Embedded C
                                Primary Application: Sensor Data Acquisition and Vibration Detection
                                Project: CDAC Academic Project

