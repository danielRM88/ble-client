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
#include <esp_wifi.h>

extern "C" {
	#include "http_request_handler.h"
	void app_main(void);
}

static char LOG_TAG[] = "BLEClient";
static WiFi *wifi;
static const int NUMBER_OF_BEACONS = 2;
uint32_t connectedCount;
BLEAddress addresses[NUMBER_OF_BEACONS];

static BLEUUID serviceUUID("91bad492-b950-4226-aa2b-4ede9fa42f59");
static BLEUUID    charUUID("0d563a58-196a-48ce-ace2-dfec78acc814");

class MyClient: public Task {
	void run(void* data) {
		BLEAddress address;
		std::stringstream jsonBody;
		std::stringstream size;
		std::size_t jsonBodySize;
		int rssiValue = 0;
		RESTClient client;
		bool info;
		BLEClient* pClient = BLEDevice::createClient();

		while (true) {
			info = false;
			jsonBody.str(std::string());
			jsonBody.clear();
			size.str(std::string());
			size.clear();
			jsonBody << "{\"values\": [";
			for(int i=0; i<NUMBER_OF_BEACONS; i++) {
				address = addresses[i];
				ESP_LOGI(LOG_TAG, "ADDRESS %d: %s", i, address.toString().c_str());
				pClient->connect(address);
				if(pClient->isConnected()) {
					info = true;
					rssiValue = pClient->getRssi();
					jsonBody << "{\"rssi\": " << rssiValue << ", \"mac_address\": \"" << address.toString() << "\"}";
					if (i+1 != NUMBER_OF_BEACONS) {
						jsonBody << ", ";
					}
				}
				do {
					pClient->disconnect();
				} while(pClient->isConnected());
			}

			jsonBodySize = jsonBody.str().size();
			if (info && jsonBody.str().substr(jsonBodySize-2) == ", ") {
				ESP_LOGI(LOG_TAG, "REMOVING COMMA");
				jsonBody.seekp(jsonBodySize-2);
			}

			jsonBody << "]}";
			jsonBodySize = jsonBody.str().size();
			if (info) {
				ESP_LOGI(LOG_TAG, "STARTING HTTP REQUEST ...");
				std::string buf("POST ");
				buf.append(WEB_URL);
				buf.append(" HTTP/1.1\r\n");
				buf.append("Host: ");
				buf.append(WEB_SERVER);
				buf.append(":");
				buf.append(WEB_PORT);
				buf.append("/\r\n");
				buf.append("Content-Type: application/json");
				buf.append("\r\n");
				buf.append("Content-Length:");
				size << " " << jsonBodySize;
				buf.append(size.str());
				buf.append("\r\n\r\n");
				buf.append(jsonBody.str());
				const char* req = buf.c_str();
				ESP_LOGI(LOG_TAG, "REQUEST: \r\n%s", req);
				http_send_request(req);
				ESP_LOGI(LOG_TAG, "REQUEST SENT!");
			} else {
				ESP_LOGI(LOG_TAG, "COULD NOT CONNECT TO ANY BEACONS");
			}
			FreeRTOS::sleep(200);
		}
	}
};

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
	void onResult(BLEAdvertisedDevice advertisedDevice) {
		ESP_LOGI(LOG_TAG, "Advertised Device: %s", advertisedDevice.toString().c_str());

		if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {
			addresses[connectedCount] = advertisedDevice.getAddress();
			connectedCount++;
			ESP_LOGI(LOG_TAG, "Found our device!  address: %s", advertisedDevice.getAddress().toString().c_str());
		}

		if(connectedCount == NUMBER_OF_BEACONS) {
			advertisedDevice.getScan()->stop();
			ESP_LOGI(LOG_TAG, "STOPPED SCAN!");
			MyClient* pMyClient = new MyClient();
			pMyClient->setStackSize(18000);
			pMyClient->start();
		}
	}
};

class MyWiFiEventHandler: public WiFiEventHandler {

	esp_err_t staGotIp(system_event_sta_got_ip_t event_sta_got_ip) {
		ESP_LOGI(LOG_TAG, "MyWiFiEventHandler(Class): staGotIp");

		return ESP_OK;
	}

	esp_err_t staDisconnected(system_event_sta_disconnected_t info) {
		ESP_LOGI(LOG_TAG, "DISCONNECTED");
		esp_restart();

		return ESP_OK;
	}
};

void run() {
	MyWiFiEventHandler *eventHandler = new MyWiFiEventHandler();
	wifi = new WiFi();
	wifi->setWifiEventHandler(eventHandler);

	BLEDevice::init("");
	BLEDevice::setPower(ESP_PWR_LVL_P7);
	ESP_LOGI(LOG_TAG, "BEGGINING OF LOOP");
	wifi->connectAP(WIFI_SSID, WIFI_PASSWORD);

	BLEScan *pBLEScan = BLEDevice::getScan();
	pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
	pBLEScan->setActiveScan(true);
	pBLEScan->start(15);
}

void app_main(void) {
	run();
}
