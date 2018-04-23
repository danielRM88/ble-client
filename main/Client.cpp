/*
 * Client.cpp
 *
 *  Created on: Apr 23, 2018
 *      Author: danielrosato
 */

#include <esp_log.h>
#include <BLEUtils.h>
#include <BLEAdvertisedDevice.h>
#include <Task.h>
#include <BLEClient.h>
#include <BLEScan.h>
#include <BLEDevice.h>
#include <WiFi.h>
#include <WiFiEventHandler.h>
#include <curl/curl.h>
#include <RESTClient.h>
#include <string>
#include <sstream>

extern "C" {
	#include "http.h"
}

static const char* LOG_TAG = "Client";

static BLEUUID service1UUID("12345678-9ABC-DEF1-2345-6789ABCDEF12");
static BLEUUID service2UUID("91bad492-b950-4226-aa2b-4ede9fa42f59");

static WiFi *wifi;
//#define WIFI_SSID 	  "Hall_Of_Residence"
//#define WIFI_PASSWORD "hofr6971"
#define WIFI_SSID 	  "Daniel's iPhone"
#define WIFI_PASSWORD "daniel'sPASSWORD!?"

static BLEAddress *server1;
static BLEAddress *server2;
//static BLEAddress *server3;

static BLEScan *pBLEScan;

static int connected = 0;

class MyClient: public Task {
	void run(void* data) {
		ESP_LOGI(LOG_TAG, "TASK STARTED!");
		RESTClient client;
		BLEClient*  pClient1  = BLEDevice::createClient();

		int rssi1;
		int rssi2=0;
		while(1) {
			ESP_LOGI(LOG_TAG, "INSIDE WHILE!");
			if (connected == 1) {

				ESP_LOGI(LOG_TAG, "INSIDE CONNECTED!");
				pClient1->connect(*server1);
				rssi1 = pClient1->getRssi();
				pClient1->disconnect();

				pClient1->connect(*server2);
				rssi2 = pClient1->getRssi();
				pClient1->disconnect();

				std::ostringstream payload;
				payload << "rssi1=" << rssi1 << "&rssi2=" << rssi2;

//				RESTTimings *timings = client.getTimings();
//
//				client.setURL("http://172.20.10.3:3000");
//				client.addHeader("Content-Type", "application/json");
//				client.post(payload.str());
//				ESP_LOGD(LOG_TAG, "Result: %s", client.getResponse().c_str());
//				timings->refresh();
//				ESP_LOGD(LOG_TAG, "timings: %s", timings->toString().c_str());

				std::string buf("GET ");
				buf.append("/register?");
				buf.append(payload.str());
				buf.append(" HTTP/1.1\r\n");
				buf.append("Host: 172.20.10.3:3000/\r\n");
				buf.append("User-Agent: esp-idf/1.0 esp32\r\n");
				buf.append("\r\n");

				const char* req = buf.c_str();

				ESP_LOGI(LOG_TAG, "Request: %s", req);
				send_req(req);


				FreeRTOS::sleep(1000);
			} else {
//				esp_restart();
				FreeRTOS::sleep(100);
			}
		}
	}
};

/**
 * Scan for BLE servers and find the first one that advertises the service we are looking for.
 */
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
	/**
	 * Called for each advertising BLE server.
	 */
	void onResult(BLEAdvertisedDevice advertisedDevice) {
		ESP_LOGI(LOG_TAG, "Advertised Device: %s", advertisedDevice.toString().c_str());

		if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(service1UUID) && !server1) {
			ESP_LOGI(LOG_TAG, "Found our 1st device!  address: %s", advertisedDevice.getAddress().toString().c_str());
			server1 = new BLEAddress(*advertisedDevice.getAddress().getNative());
		} else if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(service2UUID) && !server2) {
			ESP_LOGI(LOG_TAG, "Found our 2nd device!  address: %s", advertisedDevice.getAddress().toString().c_str());
			server2 = new BLEAddress(*advertisedDevice.getAddress().getNative());
		}

		ESP_LOGI(LOG_TAG, "CHECK FOR DEVICES!");
		if(server1 != NULL && server2 != NULL) {
			ESP_LOGI(LOG_TAG, "STARTING TASK!");
			advertisedDevice.getScan()->stop();
			MyClient* pMyClient = new MyClient();
			pMyClient->setStackSize(18000);
			pMyClient->start();
		}
	} // onResult
}; // MyAdvertisedDeviceCallbacks

class MyWiFiEventHandler: public WiFiEventHandler {

	esp_err_t staGotIp(system_event_sta_got_ip_t event_sta_got_ip) {
		ESP_LOGD(LOG_TAG, "MyWiFiEventHandler(Class): staGotIp");
		connected = 1;

		return ESP_OK;
	}

	esp_err_t staDisconnected(system_event_sta_disconnected_t info) {
		ESP_LOGD(LOG_TAG, "DISCONNECTED");
		esp_restart();

		return ESP_OK;
	}
};

void Client(void) {
	MyWiFiEventHandler *eventHandler = new MyWiFiEventHandler();
	wifi = new WiFi();
	wifi->setWifiEventHandler(eventHandler);
	wifi->connectAP(WIFI_SSID, WIFI_PASSWORD);

	BLEDevice::init("");
	pBLEScan = BLEDevice::getScan();
	pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
	pBLEScan->setActiveScan(true);
	pBLEScan->start(15);
}

