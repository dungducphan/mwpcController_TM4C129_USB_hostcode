#include <iostream>
#include "libusb.h"
#include <unistd.h>

unsigned int microseconds = 500;

typedef enum {
    DATA_NOT_READY = 0,
    DATA_READY
} DEVICE_STATE_t;

int main(void) {
    libusb_device_handle *dev_handle = NULL;
    libusb_device **list;
    libusb_context *ctx = NULL; // a libusb session
    int r; // for return values
    ssize_t cnt; // holding number of devices in list

    // Initialize library
    r = libusb_init(&ctx);
    if (r < 0) {
        std::cout << "Init Error. Error code: " << r << "." << std::endl;
        return 1;
    }

    //set verbosity level to 3, as suggested in the documentation
    libusb_set_debug(ctx, 3);

    // Get list of USB devices currently connected
    cnt = libusb_get_device_list(ctx, &list);
    if (cnt < 0) {
        std::cout << "Get Device Error. " << std::endl;
        return 1;
    }
    std::cout << cnt << " devices found in the list." << std::endl;

    // vendorID (0x1cbe = 7358) and productID (0x0003 = 3) for the wire chamber controller
    dev_handle = libusb_open_device_with_vid_pid(ctx, 7358, 3);
    if (dev_handle == NULL) std::cout << "Cannot open device." << std::endl;
    else std::cout << "Device Opened Successfully." << std::endl;
    libusb_free_device_list(list, 1);

    // find out if kernel driver is attached
    if (libusb_kernel_driver_active(dev_handle, 0) == 1) {
        std::cout << "Kernel Driver Active." << std::endl;
        // detach it
        if (libusb_detach_kernel_driver(dev_handle, 0) == 0)
        std::cout << "Kernel Driver Detached!" << std::endl;
    }

    // claim interface 0 (the first) of device
    r = libusb_claim_interface(dev_handle, 0);
    if (r < 0) {
        std::cout << "Cannot Claim Interface. Error code: " << r << "." << std::endl;
        return 1;
    }
/*
    uint8_t *send_data = new uint8_t[1]; // data to write
    int send_actual; // used to find out how many bytes were written
    uint8_t *read_data = new uint8_t[69]; // data from device
    int read_actual; // used to find out how many bytes were read
    DEVICE_STATE_t dev_state = DATA_NOT_READY;
    int send_return;
    int read_return;
    while (1) {

        if (dev_state == DATA_NOT_READY) {
            send_data[0] = 114; // DATA_AVAILABILITY_QUERY

            // the controller's OUT endpointID is 0x01, IN endpointID is 0x81
            // find this information by "lsusb -v -d 1cbe:0003"
            send_return = libusb_bulk_transfer(dev_handle,
                                                 (0x01 | LIBUSB_ENDPOINT_OUT),
                                                 send_data,
                                                 1,
                                                 &send_actual,
                                                 0);
            if (send_return == 0 && send_actual == 1) std::cout << "Writing Successful!" << std::endl;
            else std::cout << "Write Error. Error code: " << r << "." << std::endl;

            // read back
            read_return = libusb_bulk_transfer(dev_handle,
                                                 (0x81 | LIBUSB_ENDPOINT_OUT),
                                                 read_data,
                                                 69,
                                                 &read_actual,
                                                 0);
            if (read_return == 0) {
                if (read_actual == 5) {
                    std::cout << "Read Back Expected. Message: " << read_data[0] << "-"
                              << read_data[1] << "-"
                              << read_data[2] << "-"
                              << read_data[3] << "-"
                              << read_data[4] << std::endl;
                }
            } else {
                std::cout << "Read Back Error. Error code: " << r << "." << std::endl;
            }

            usleep(microseconds);
        }
    }
*/

    while (1) {}

//    // release the claimed interface
//    r = libusb_release_interface(dev_handle, 0);
//    if (r != 0) {
//        std::cout << "Cannot Release Interface. Error code: " << r << "." << std::endl;
//        return 1;
//    }
//    std::cout << "Released Interface." << std::endl;
//
//    libusb_close(dev_handle);
//    libusb_exit(ctx);
//    delete[] send_data;
//    delete[] read_data;

    return 0;
}
