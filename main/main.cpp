/*
 * Test the REST API client.
 * This application leverages LibCurl.  You must make that package available
 * as well as "enable it" from "make menuconfig" and C++ Settings -> libCurl present.
 *
 * You may also have to include "posix_shims.c" in your compilation to provide resolution
 * for Posix calls expected by libcurl that aren't present in ESP-IDF.
 *
 * See also:
 * * https://github.com/nkolban/esp32-snippets/issues/108
 *
 */
#include <curl/curl.h>
#include <esp_log.h>
#include <string>
#include <sstream>
#include "BLEDevice.h"

#include "BLEAdvertisedDevice.h"
#include "BLEClient.h"
#include "BLEScan.h"
#include "BLEUtils.h"
#include "Task.h"


#include "sdkconfig.h"

static char LOG_TAG[] = "main";
static const int BUILT_IN_LED = 2;
static const int ON = 1;
static const int OFF = 1;

#define uS_TO_S_CONVERSION 1000000
#define TIME_TO_SLEEP 3

extern "C" {
	void app_main(void);
}

static BLEUUID serviceUUID("91bad492-b950-4226-aa2b-4ede9fa42f59");
// The characteristic of the remote service we are interested in.
static BLEUUID    charUUID("0d563a58-196a-48ce-ace2-dfec78acc814");
//static BLEUUID    charUUID("12345678-9ABC-DEF1-2345-6789ABCDEF12");

//BLEClient*  pClient;

RTC_DATA_ATTR BLEAddress* pAddress;

//static void notifyCallback(
//	BLERemoteCharacteristic* pBLERemoteCharacteristic,
//	uint8_t* pData,
//	size_t length,
//	bool isNotify) {
////		std::string value = pBLERemoteCharacteristic->readValue();
//		ESP_LOGI(LOG_TAG, "Notify callback for characteristic %s of data length %d",
//				pBLERemoteCharacteristic->getUUID().toString().c_str(), length);
//
////		ESP_LOGI(LOG_TAG, "The characteristic value was: %d", value.c_str());
//
//		int rssi;
////		if(value.compare("send") == 0) {
//			std::ostringstream stringStream;
//			rssi = pClient->getRssi();
//			stringStream << rssi;
//			pBLERemoteCharacteristic->writeValue(stringStream.str());
////		}
//}

/**
 * Become a BLE client to a remote BLE server.  We are passed in the address of the BLE server
 * as the input parameter when the task is created.
 */
class MyClient: public Task {
	void run(void* data) {
		pAddress = (BLEAddress*)data;
		BLEClient *pClient  = BLEDevice::createClient();

		// Connect to the remove BLE Server.
		pClient->connect(*pAddress);

		// Obtain a reference to the service we are after in the remote BLE server.
		BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
		if (pRemoteService == nullptr) {
			ESP_LOGD(LOG_TAG, "Failed to find our service UUID: %s", serviceUUID.toString().c_str());
			return;
		}


		// Obtain a reference to the characteristic in the service of the remote BLE server.
		BLERemoteCharacteristic* pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
		if (pRemoteCharacteristic == nullptr) {
			ESP_LOGD(LOG_TAG, "Failed to find our characteristic UUID: %s", charUUID.toString().c_str());
			return;
		}

		int rssi = 0;
		while(1) {
			// Set a new value of the characteristic

			// Read the value of the characteristic.
//			std::string value = pRemoteCharacteristic->readValue();
//			ESP_LOGD(LOG_TAG, "The characteristic value was: %s", value.c_str());

//			if(value.compare("send") == 0) {
//				std::ostringstream stringStream;
			rssi = pClient->getRssi();
			ESP_LOGD(LOG_TAG, "RSSI %d", rssi);
//				stringStream << rssi;
//				pRemoteCharacteristic->writeValue(stringStream.str());
			if(!pClient->isConnected()){
				ESP_LOGD(LOG_TAG, "LOST CONNECTION!");
				GPIO_OUTPUT_SET( BUILT_IN_LED, ON );
				pClient->disconnect();
				pClient->connect(*pAddress);
				GPIO_OUTPUT_SET( BUILT_IN_LED, OFF );
				ESP_LOGD(LOG_TAG, "CONNECTED!");
			}

			FreeRTOS::sleep(5000);
//			ESP_LOGD(LOG_TAG, "BEFORE SLEEP!");
//			esp_light_sleep_start();
//			ESP_LOGD(LOG_TAG, "AFTER SLEEP!");
//			} else {
//				FreeRTOS::sleep(1000);
//			}
		}

		pClient->disconnect();

//		// Read the value of the characteristic.
//		std::string value = pRemoteCharacteristic->readValue();
//		ESP_LOGD(LOG_TAG, "The characteristic value was: %s", value.c_str());

//		pRemoteCharacteristic->registerForNotify(notifyCallback);

		ESP_LOGD(LOG_TAG, "%s", pClient->toString().c_str());
		ESP_LOGD(LOG_TAG, "-- End of task");
	} // run
}; // MyClient

/**
 * Scan for BLE servers and find the first one that advertises the service we are looking for.
 */
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
	/**
	 * Called for each advertising BLE server.
	 */
	void onResult(BLEAdvertisedDevice advertisedDevice) {
		ESP_LOGI(LOG_TAG, "Advertised Device: %s", advertisedDevice.toString().c_str());

		if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {
			GPIO_OUTPUT_SET( BUILT_IN_LED, ON );
			FreeRTOS::sleep(500);
			GPIO_OUTPUT_SET( BUILT_IN_LED, OFF );
			advertisedDevice.getScan()->stop();

			ESP_LOGI(LOG_TAG, "Found our device!  address: %s", advertisedDevice.getAddress().toString().c_str());
			MyClient* pMyClient = new MyClient();
			pMyClient->setStackSize(18000);
			pMyClient->start(new BLEAddress(*advertisedDevice.getAddress().getNative()));
		} // Found our server
	} // onResult
}; // MyAdvertisedDeviceCallbacks

void app_main(void) {

	GPIO_OUTPUT_SET( BUILT_IN_LED, OFF );
	ESP_LOGD(LOG_TAG, "Scanning sample starting");
	BLEDevice::init("");
	BLEDevice::setPower(ESP_PWR_LVL_P7);
	BLEScan *pBLEScan = BLEDevice::getScan();
	pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
	pBLEScan->setActiveScan(true);
	pBLEScan->start(15);
	esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_CONVERSION);
}
