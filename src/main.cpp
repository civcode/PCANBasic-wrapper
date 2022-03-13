#include "PCANBasicWrapper.h"
#include "SimpleTimer.h"

#include <iostream>
#include <thread>

using std::cout;
using std::endl;

int main() {

    PCANBasicWrapper pcan;
    SimpleTimer loop_timer(std::chrono::seconds(5));

    pcan.SearchForDevices();

    if (true) {
        if (!pcan.InitializeByUsbBus(PCAN_USBBUS1, PCAN_BAUD_250K)) {
            cout << "Could not initialize PCAN-USB" << endl;
            return 0;
        }
    } else {
        if (!pcan.InitializeByDeviceId(0xf0, PCAN_BAUD_500K)) {
            cout << "Could not initialize PCAN-USB" << endl;
            return 0;
        }
    }

    std::thread worker(PCANBasicWrapper::worker, &pcan);

    //while(!loop_timer.is_expired()) {
    while(true) {
        
        PCANBasicWrapper::can_msg msg = {
            .id = 0x18FFA017,
            .len = 4,
            .data = {1,2,3,4,5,6,7,8}
        };
        
        pcan.Write(&msg);

        //PCANBasicWrapper::can_msg msg_rx;
        //pcan.Read(&msg_rx);

        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    }

    cout << "stopping worker therad" << endl;
    pcan.stop_worker();
    worker.join();

    return 0;
}