#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/usb.h>
#include<linux/slab.h>  // required for kmalloc/kzalloc  
#include <linux/fs.h> 
#include <linux/types.h>
#include<linux/uaccess.h> //copy to user 


#define USB_VENDOR_ID 0X0483   //STM32 default VID 
#define USB_PRODUCT_ID 0x5740  //STM32 PID 

#define BULK_BUFFER_SIZE 512    //usb data buffer size 
//device specific structure

struct mpu6050
{
    struct usb_device *udev ;
    struct  usb_interface *interface;
    unsigned char *bulk_in_buffer;
    size_t bulk_in_size ;   
    __u8 bulk_in_endpointAddr;
    
    //__u8 bulk_out_endpointAddr;
};

//table of devices  supporetd by this devices 

static struct usb_device_id mpu_table[] =
{
    {
        USB_DEVICE(USB_VENDOR_ID, USB_PRODUCT_ID)
    },

    {}
};

MODULE_DEVICE_TABLE(usb, mpu_table); // export usbID table 

static int mpu_usb_read (struct mpu6050 *dev){
 
    int actual_length ;
    int ret ;
    
    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;
    
    ret = usb_bulk_msg( dev->udev, usb_rcvbulkpipe(dev->udev,dev->bulk_in_endpointAddr),dev->bulk_in_buffer,dev->bulk_in_size,&actual_length,5000);

    if (ret)
    {
         pr_err("usb_bulk_msg failed ret=%d\n", ret);
         return ret ;
    }
if (actual_length < 6)
{
    pr_err("Not enough data received\n");
    return -EIO;
}
    pr_info("Received %d bytes\n", actual_length);

accel_x = (int16_t)((dev->bulk_in_buffer[0] << 8) | dev->bulk_in_buffer[1]);
accel_y = (int16_t)((dev->bulk_in_buffer[2] << 8) | dev->bulk_in_buffer[3]);
accel_z = (int16_t)((dev->bulk_in_buffer[4] << 8) | dev->bulk_in_buffer[5]);

pr_info("Accel X=%d Y=%d Z=%d\n",accel_x, accel_y, accel_z);
    
    return 0;
}

static int mpu_probe(struct usb_interface *interface,const struct usb_device_id *id)
{
    struct mpu6050 *dev;
    struct usb_host_interface *iface_desc;
    struct usb_endpoint_descriptor *endpoint;
    int i;

    pr_info("Probe function is called for custom USB\n");

    //Allocate memory for device structure 
    dev = kzalloc(sizeof(*dev), GFP_KERNEL);
    if (!dev)
    {
        return -ENOMEM;
    }
    
    // Store USB device information 
    dev->udev = usb_get_dev(interface_to_usbdev(interface));
    dev->interface = interface;

    // Get current interface descriptor 
    iface_desc = interface->cur_altsetting;

    // Ignore CDC Communication interface 
    if (iface_desc->desc.bInterfaceNumber != 1)
    {
        dev_info(&interface->dev,
                 "Ignoring interface %d\n",
                 iface_desc->desc.bInterfaceNumber);

        usb_put_dev(dev->udev);
        kfree(dev);
        return -ENODEV;
    }
  
  dev_info(&interface->dev,"CDC Data interface 1 detected\n");

    // Find Bulk IN endpoint 
    for (i = 0; i < iface_desc->desc.bNumEndpoints; i++)
    {
        endpoint = &iface_desc->endpoint[i].desc;

        if (usb_endpoint_is_bulk_in(endpoint))
        {
            // Store endpoint address 
            dev->bulk_in_endpointAddr =
                endpoint->bEndpointAddress;

            //Get maximum packet size 
            dev->bulk_in_size =
                le16_to_cpu(endpoint->wMaxPacketSize);

            // Allocate memory for incoming data buffer 
            dev->bulk_in_buffer =
                kmalloc(dev->bulk_in_size, GFP_KERNEL);

            if (!dev->bulk_in_buffer)
            {
                usb_put_dev(dev->udev);
                kfree(dev);
                return -ENOMEM;
            }

            dev_info(&interface->dev,
                     "Bulk IN endpoint found: 0x%02X\n",
                     dev->bulk_in_endpointAddr);

           dev_info(&interface->dev,
                     "Bulk IN buffer size: %zu bytes\n",
                     dev->bulk_in_size);

            break;
        }
    }

    // Bulk IN endpoint was not found 
    if (!dev->bulk_in_buffer)
    {
        dev_err(&interface->dev,
                "Bulk IN endpoint not found\n");

        usb_put_dev(dev->udev);
        kfree(dev);
        return -ENODEV;
    }

    // Save device structure in interface 
    usb_set_intfdata(interface, dev);

    // Device is ready 
    dev_info(&interface->dev,
             "MPU6050 USB device is connected\n");

    // Read MPU6050 data
    mpu_usb_read(dev);

    return 0;
}

//usb disconnect function 

static void mpu_disconnect(struct usb_interface *interface)
{
    struct mpu6050 *dev;
    dev = usb_get_intfdata(interface);

    if(dev){

        usb_set_intfdata(interface , NULL);
        kfree(dev->bulk_in_buffer);
        if(dev->udev){

            usb_put_dev(dev->udev);
        }
        kfree(dev);
    }
    dev_info(&interface->dev,"USB device is disconnected\n");
}

//USB driver structure 

static struct usb_driver mpu_driver ={
      
     .name = "mpu6050_usb",
     .probe = mpu_probe ,
     .disconnect = mpu_disconnect,
     .id_table = mpu_table,
};


module_usb_driver(mpu_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("RAMAN PARGE<reddyram6543@gmail.com>");
MODULE_DESCRIPTION("custom STM32 MPU6050 USB driver");
