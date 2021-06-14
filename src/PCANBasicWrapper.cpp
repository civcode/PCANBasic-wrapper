#include "PCANBasicWrapper.h"

#include <cstdio>
#include <iostream>
#include <thread>
#include <chrono>

using std::vector;
using std::cout;
using std::endl;

//PCANBasicWrapper::PCANBasicWrapper(int pcan_usb_bus, int pcan_baud_rate, int pcan_device_id) {}
PCANBasicWrapper::PCANBasicWrapper() : is_worker_running_(false) {
    //const vector<int> tmp{PCAN_USBBUS1, PCAN_USBBUS2, PCAN_USBBUS3, 
    //                      PCAN_USBBUS4, PCAN_USBBUS5, PCAN_USBBUS6};
    //usb_bus_list_ = &tmp;

    pcan_usb_bus_ = -1;
}

bool PCANBasicWrapper::InitializeByUsbBus(int pcan_usb_bus, int pcan_baud_rate) {
  
    pcan_usb_bus_ = pcan_usb_bus;

    TPCANStatus result;
    char strMsg[256];

    cout << "Initializeing PCAN device" << endl;
    result = CAN_Initialize(pcan_usb_bus, pcan_baud_rate, 0, 0, 0);
	
    if (result != PCAN_ERROR_OK) {
        // An error occurred, get a text describing the error and show it
        CAN_GetErrorText(result, 0, strMsg);
        cout << strMsg << endl;
        return false;
    } else {
        cout << "PCAN-USB " << GetUsbBusNumber(pcan_usb_bus) << " was initialized" << endl;
        is_initialized_ = true;
    } 
	
    // Read all buffered messages from other USB2CAN Dongles
    PurgeExternalFrameBuffer();

	// Clear receive and transmit buffer
	result = CAN_Reset(pcan_usb_bus);
	if(result != PCAN_ERROR_OK)
	{
		// An error occurred, get a text describing the error and show it
		CAN_GetErrorText(result, 0, strMsg);
        cout << strMsg << endl;
        return false;
	}
	else {
        cout << "PCAN-USB " << GetUsbBusNumber(pcan_usb_bus) << " was reset" << endl;
    }


    return true;
}

bool PCANBasicWrapper::InitializeByDeviceId(int pcan_device_id, int pcan_baud_rate) {

    // Read all buffered messages from other USB2CAN Dongles
    PurgeExternalFrameBuffer();

    return true;
}


bool PCANBasicWrapper::Write(const can_msg* msg) {

    TPCANMsg pcan_msg;
    TPCANStatus result;
    char strMsg[256];
	
	// A CAN message is configured
    pcan_msg.ID = static_cast<DWORD>(msg->id);
    pcan_msg.MSGTYPE = PCAN_MESSAGE_EXTENDED;
	pcan_msg.LEN = msg->len;
	for (int i=0; i<msg->len; i++) {
		pcan_msg.DATA[i] = msg->data[i];
	}

	// The message is sent using the PCAN-USB Channel 1
	result = CAN_Write(pcan_usb_bus_, &pcan_msg);
	if(result != PCAN_ERROR_OK)
	{
		// An error occurred, get a text describing the error and show it
		CAN_GetErrorText(result, 0, strMsg);
		cout << strMsg << endl;
        return false;
	}
	else {
		cout << "message transmitted \n";
	}

    return true;
}

bool PCANBasicWrapper::Read(can_msg* msg) {
    // pcan read message
    TPCANMsg pcan_msg;
    TPCANTimestamp timestamp;
    TPCANStatus result;
    char strMsg[256];

    result = CAN_Read(pcan_usb_bus_, &pcan_msg, &timestamp);
    if (result == PCAN_ERROR_OK) {
        // populate frame
        msg->id = pcan_msg.ID;
        msg->len = pcan_msg.LEN;
        for (int i=0; i<pcan_msg.LEN; i++) {
            msg->data[i] = pcan_msg.DATA[i];
        }
        cout << "message received" << endl;
    } else {
        // An error occurred, get a text describing the error and show it
        // and handle the error
        CAN_GetErrorText(result, 0, strMsg);
        switch (result) {
            case PCAN_ERROR_QRCVEMPTY: 
            case PCAN_ERROR_OK:
            break;

            default:
                cout << strMsg << endl;
        }
        return false;
    }
    return true;
}


void PCANBasicWrapper::worker() {
    is_worker_running_ = true;
    while (is_worker_running_) {
        PCANBasicWrapper::can_msg msg_rx;
        Read(&msg_rx);

        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        //if (!is_worker_running_) {
        //    cout << "here" << endl;
        /// }
    }
}

void PCANBasicWrapper::stop_worker() {
    is_worker_running_ = false;
    //cout << "is_worker_running_ = false" << endl;
}

void PCANBasicWrapper::SearchForDevices() {

    //TPCANStatus result;
    int buffer;

    cout << "Searching for PCAN dongles" << endl;

    //const vector<int> usb_bus_list{PCAN_USBBUS1, PCAN_USBBUS2, PCAN_USBBUS3, PCAN_USBBUS4, PCAN_USBBUS5, PCAN_USBBUS6};

    for (auto it=usb_bus_list_.begin(); it!=usb_bus_list_.end(); ++it) {
        int idx = std::distance(usb_bus_list_.begin(), it);
        if (CAN_Initialize(*it, PCAN_DONGLE_BAUD_RATE, 0, 0, 0) == PCAN_ERROR_OK) {
            if (CAN_GetValue(*it, PCAN_DEVICE_ID, &buffer, sizeof(buffer)) == PCAN_ERROR_OK) {
                cout << "Channel " << idx << ": " << "Device Id: 0x" << std::hex << buffer << endl;
            } 
		    CAN_Uninitialize(*it);
        } else {
                cout << "Channel " << idx << ": " << "error" << endl;
        }
    }
}

int PCANBasicWrapper::GetUsbBusNumber(int pcan_usb_bus) {
    int bus_number = -1;
    for (auto it=usb_bus_list_.begin(); it!=usb_bus_list_.end(); ++it) {
        if (*it == pcan_usb_bus) {
            bus_number = std::distance(usb_bus_list_.begin(), it);
        }
    }
    return bus_number;
}

void PCANBasicWrapper::PurgeExternalFrameBuffer() {
    TPCANMsg pcan_msg;
    TPCANTimestamp timestamp;
    TPCANStatus result;
    char strMsg[256];

    cout << "Purging external frame buffers" << endl;

    int cnt = 0;

    while (CAN_Read(pcan_usb_bus_, &pcan_msg, &timestamp) == PCAN_ERROR_OK) {
        // do nothing
        cnt++;
    }

    cout << "Purged " << cnt << " messages" << endl;
}