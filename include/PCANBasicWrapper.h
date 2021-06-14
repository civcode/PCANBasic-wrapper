#ifndef PCANBASIC_WRAPPER_H_
#define PCANBASIC_WRAPPER_H_

#include "PCANBasic.h"

#include <cstdint>
#include <vector>

using std::vector;

#define PCAN_DONGLE_DEVICE_ID (0x21)
#define PCAN_DONGLE_BAUD_RATE PCAN_BAUD_250K

class PCANBasicWrapper {

public:
    typedef struct can_mgs_ {
        uint32_t id;
        uint8_t len;
        uint8_t data[8];

    } can_msg;

    //PCANBasicWrapper(int pcan_usb_bus, int pcan_baud_rate, int pcan_device_id=-1);  
    PCANBasicWrapper();  

    bool InitializeByUsbBus(int pcan_usb_bus, int pcan_baud_rate);
    bool InitializeByDeviceId(int pcan_device_id, int pcan_baud_rate);
    bool Read(can_msg* msg);
    bool Write(const can_msg* msg);
    void SearchForDevices();

    void worker();
    void stop_worker();




private:
    bool is_initialized_;
    bool is_worker_running_;
    int pcan_usb_bus_;
    //const 
    //const int usb_bus_list_[4];
    const vector<int> usb_bus_list_{PCAN_USBBUS1, PCAN_USBBUS2, 
                                    PCAN_USBBUS3, PCAN_USBBUS4, 
                                    PCAN_USBBUS5, PCAN_USBBUS6};

    int GetUsbBusNumber(int pcan_usb_bus);
    void PurgeExternalFrameBuffer();



};



#endif // PCANBASIC_WRAPPER_H_