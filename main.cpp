#include <iostream>
#include "libusb.h"
#include <unistd.h>
#include <vector>

void ProcessData(uint8_t *read_data) {
    int32_t token1 = read_data[0] + read_data[1] * 256 + read_data[2] * 65536 + read_data[3] * 16777216;
    int32_t shotID = read_data[4] + read_data[5] * 256 + read_data[6] * 65536 + read_data[7] * 16777216;
    int32_t token2 = read_data[8] + read_data[9] * 256 + read_data[10] * 65536 + read_data[11] * 16777216;
    int32_t token3 = read_data[76] + read_data[77] * 256 + read_data[78] * 65536 + read_data[79] * 16777216;

    std::cout << "Shot   : " << shotID << std::endl;
    std::cout << "Token_1: " << token1 << std::endl;
    std::cout << "Token_2: " << token2 << std::endl;
    std::cout << "Token_3: " << token3 << std::endl;
    for (uint8_t i = 0; i < 32; i++) {
        int16_t val = read_data[12 + i * 2 + 0] + read_data[12 + i * 2 + 1] * 256;
        printf("Channel [%02i]: %04i\n", i, val);
    }
}

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
    // the controller's OUT endpointID is 0x01, IN endpointID is 0x81
    // find this information by "lsusb -v -d 1cbe:0003"
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

    uint8_t *send_data = new uint8_t[1]; // data to write
    int send_actual; // used to find out how many bytes were written
    uint8_t *read_data = new uint8_t[80]; // data from device
    int read_actual; // used to find out how many bytes were read
    int read_return;

    int command = 0;
    do {
        send_data[0] = (uint8_t) 114; // DATA_AVAILABILITY_QUERY
        do {
            r = libusb_bulk_transfer(dev_handle,
                                     (0x01 | LIBUSB_ENDPOINT_OUT),
                                     send_data,
                                     1,
                                     &send_actual,
                                     0);

            read_return = libusb_bulk_transfer(dev_handle,
                                               (0x81 | LIBUSB_ENDPOINT_OUT),
                                               read_data,
                                               80,
                                               &read_actual,
                                               0);
            if (read_actual == 12) {
                printf("Data N/A.\n");
                usleep(500);
            }
        } while (read_actual == 12);

        if (read_actual == 80) {
            ProcessData(read_data);
        } else {
            std::cout << "Error. Read Actual: " << read_actual << std::endl;
        }

        usleep(95 * 1000);

        command++;
    } while (command < 1000);

    // release the claimed interface
    r = libusb_release_interface(dev_handle, 0);
    if (r != 0) {
        std::cout << "Cannot Release Interface. Error code: " << r << "." << std::endl;
        return 1;
    }
    std::cout << "Released Interface." << std::endl;

    libusb_close(dev_handle);
    libusb_exit(ctx);
    delete[] send_data;
    delete[] read_data;

    return 0;
}
