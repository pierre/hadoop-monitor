#include <stdio.h>
#include <libusb.h>

#ifdef DEBUG
# define dprintf printf
#else
# define dprintf(...) ((void)0)
#endif

static void find_lamp(libusb_device **devs)
{
    libusb_device *dev;
    int i = 0;

    while ((dev = devs[i++]) != NULL) {
        int ret;
        char string[256];
        struct libusb_device_descriptor desc;
        libusb_device_handle *udev = NULL;

        int r = libusb_get_device_descriptor(dev, &desc);
        if (r < 0) {
            fprintf(stderr, "failed to get device descriptor");
            continue;
        } else {
            dprintf("%04x:%04x (bus %d, device %d)\n",
                desc.idVendor, desc.idProduct,
                libusb_get_bus_number(dev), libusb_get_device_address(dev));
        }

        r = libusb_open(dev, &udev);
        if (r < 0) {
            fprintf(stderr, "failed to open usb device");
            continue;
        }

        if (desc.iManufacturer) {
            ret = libusb_get_string_descriptor_ascii(udev,
                                        desc.iManufacturer,
                                        string,
                                        sizeof(string));
            if (ret > 0)
                dprintf("­ Manufacturer : %s\n", string);
            else
                dprintf("­ Unable to fetch manufacturer string\n");
        }

        if (desc.iProduct) {
            ret = libusb_get_string_descriptor_ascii(udev,
                                        desc.iProduct,
                                        string,
                                        sizeof(string));
             if (ret > 0)
                dprintf("­ Product : %s\n", string);
             else
                dprintf("­ Unable to fetch product string\n"); 
        }
    }
}

int main(void)
{
    libusb_device **devs;
    int r;
    ssize_t cnt;

    r = libusb_init(NULL);
    if (r < 0)
        return r;
    else
        printf("libusb initialized\n");

    cnt = libusb_get_device_list(NULL, &devs);
    if (cnt < 0)
        return (int) cnt;
    else
        printf("Found usb devices\n");

    find_lamp(devs);

    printf("Cleaning up device list\n");
    libusb_free_device_list(devs, 1);

    libusb_exit(NULL);
    return 0;
}
