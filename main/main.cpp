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
#include <RESTClient.h>
#include <string>
#include <Task.h>
#include <WiFi.h>
#include <WiFiEventHandler.h>
#include <BLEAdvertisedDevice.h>
#include <BLEClient.h>
#include <BLEScan.h>
#include <BLEUtils.h>
#include <BLEClient.h>
#include <BLEDevice.h>
#include <sdkconfig.h>
#include <sstream>
#include <esp_bt.h>
#include <esp_bt_main.h>
#include <esp_wifi.h>

extern "C" {
	void app_main(void);
}

#define WIFI_SSID "Daniel's iPhone"
#define WIFI_PASSWORD "daniel'sPASSWORD!?"

#define uS_TO_S_CONVERSION 1000000
#define TIME_TO_SLEEP 0.9

static char LOG_TAG[] = "BLEClient";
static WiFi *wifi;
static const int NUMBER_OF_BEACONS = 2;
uint32_t connectedCount;
BLEAddress addresses[NUMBER_OF_BEACONS];

static BLEUUID serviceUUID("91bad492-b950-4226-aa2b-4ede9fa42f59");
static BLEUUID    charUUID("0d563a58-196a-48ce-ace2-dfec78acc814");

class CurlTestTask: public Task {
	void run(void *data) {
		ESP_LOGI(LOG_TAG, "Testing curl ...");
		RESTClient client;
		std::stringstream* jsonBody = (std::stringstream*) data;
		ESP_LOGI(LOG_TAG, "JSON BODY: %s", jsonBody->str().c_str());

		/**
		 * Test POST
		 */
		RESTTimings *timings = client.getTimings();

		client.setURL("http://172.20.10.3:3000/register");
		client.addHeader("Content-Type", "application/json");
		client.post(jsonBody->str());
		ESP_LOGI(LOG_TAG, "Result: %s", client.getResponse().c_str());
		timings->refresh();
		ESP_LOGI(LOG_TAG, "timings: %s", timings->toString().c_str());
//		FreeRTOS::sleep(1000);

		ESP_LOGI(LOG_TAG, "REQUEST SENT");
//		printf("Tests done\n");

//		esp_bluedroid_disable();
//		esp_bt_controller_disable();
//		esp_bluedroid_deinit();
//		esp_wifi_stop();
//		esp_light_sleep_start();
		return;
	}
};

class MyClient: public Task {
	void run(void* data) {
		BLEAddress address;
		std::stringstream jsonBody;
		int rssiValue = 0;
		RESTClient client;
		bool info, error;

		while (true) {
			info = false;
			error = false;
			jsonBody.str(std::string());
			jsonBody.clear();
			jsonBody << "{\"values\": [";
			for(int i=0; i<NUMBER_OF_BEACONS; i++) {
				BLEClient* pClient = BLEDevice::createClient(i);
				address = addresses[i];
				ESP_LOGI(LOG_TAG, "ADDRESS %d: %s", i, address.toString().c_str());
				// Connect to the remove BLE Server.
				pClient->connect(address);
				if(pClient->isConnected()) {
					info = true;
					rssiValue = pClient->getRssi();
					jsonBody << "{\"rssi\": " << rssiValue << ", \"mac_address\": \"" << address.toString() << "\"}";
					if (i+1 != NUMBER_OF_BEACONS) {
						jsonBody << ", ";
					}
				} else {
					error = true;
				}
				pClient->disconnect();
				delete pClient;
			}

			if (info && error) {
				ESP_LOGI(LOG_TAG, "REMOVING COMMA");
				jsonBody.seekp(jsonBody.str().size()-2);
			}

			jsonBody << "]}";
			ESP_LOGI(LOG_TAG, "JSON BODY: %s", jsonBody.str().c_str());
			if (info) {

//				ESP_LOGI(LOG_TAG, "STARTING HTTP REQUEST ...");
//
//				/**
//				 * Test POST
//				 */
//				RESTTimings *timings = client.getTimings();
//
//				client.setURL("http://172.20.10.3:3000/register");
//				client.addHeader("Content-Type", "application/json");
//				client.post(jsonBody.str());
//				ESP_LOGI(LOG_TAG, "Result: %s", client.getResponse().c_str());
//				timings->refresh();
//				ESP_LOGI(LOG_TAG, "timings: %s", timings->toString().c_str());
//
//				ESP_LOGI(LOG_TAG, "REQUEST SENT");
			} else {
				ESP_LOGI(LOG_TAG, "COULD NOT CONNECT TO ANY BEACONS");
			}
//			CurlTestTask *curlTestTask = new CurlTestTask();
//			curlTestTask->setStackSize(12000);
//			curlTestTask->start(&jsonBody);

	//		esp_light_sleep_start();
			FreeRTOS::sleep(500);
		}
	}
};

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
	/**
	 * Called for each advertising BLE server.
	 */
	void onResult(BLEAdvertisedDevice advertisedDevice) {
		ESP_LOGI(LOG_TAG, "Advertised Device: %s", advertisedDevice.toString().c_str());

		if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {
			addresses[connectedCount] = advertisedDevice.getAddress();
			connectedCount++;
			ESP_LOGI(LOG_TAG, "Found our device!  address: %s", advertisedDevice.getAddress().toString().c_str());
		} // Found our server

		if(connectedCount == NUMBER_OF_BEACONS) {
			advertisedDevice.getScan()->stop();
			ESP_LOGI(LOG_TAG, "STOPPED SCAN!");
			MyClient* pMyClient = new MyClient();
			pMyClient->setStackSize(18000);
			pMyClient->start();
		}
	} // onResult
}; // MyAdvertisedDeviceCallbacks

//static CurlTestTask *curlTestTask;

class MyWiFiEventHandler: public WiFiEventHandler {

	esp_err_t staGotIp(system_event_sta_got_ip_t event_sta_got_ip) {
		ESP_LOGI(LOG_TAG, "MyWiFiEventHandler(Class): staGotIp");

//		curlTestTask = new CurlTestTask();
//		curlTestTask->setStackSize(12000);
//		curlTestTask->start();

		return ESP_OK;
	}

	esp_err_t staDisconnected(system_event_sta_disconnected_t info) {
		ESP_LOGI(LOG_TAG, "DISCONNECTED");
		esp_restart();

		return ESP_OK;
	}
};

void run() {
	esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_CONVERSION);
	MyWiFiEventHandler *eventHandler = new MyWiFiEventHandler();
	wifi = new WiFi();
	wifi->setWifiEventHandler(eventHandler);

	BLEDevice::init("");
	BLEDevice::setPower(ESP_PWR_LVL_P7);
//	while(true) {
		ESP_LOGI(LOG_TAG, "BEGGINING OF LOOP");
		wifi->connectAP(WIFI_SSID, WIFI_PASSWORD);
	//	esp_wifi_set_ps(WIFI_PS_MAX_MODEM);

		BLEScan *pBLEScan = BLEDevice::getScan();
		pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
		pBLEScan->setActiveScan(true);
		pBLEScan->start(15);
//	}
}

void app_main(void) {
	run();
}
