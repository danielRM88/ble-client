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

#include "sdkconfig.h"

//static char tag[] = "test_rest";

extern "C" {
	void app_main(void);
}


//static WiFi *wifi;
//
//
//class CurlTestTask: public Task {
//	void run(void *data) {
//		ESP_LOGD(tag, "Testing curl ...");
//		RESTClient client;
//
//		/**
//		 * Test POST
//		 */
//
//		while(true) {
//			RESTTimings *timings = client.getTimings();
//
//			client.setURL("http://172.20.10.3:3000");
//			client.addHeader("Content-Type", "application/json");
//			client.post("hello world!");
//			ESP_LOGD(tag, "Result: %s", client.getResponse().c_str());
//			timings->refresh();
//			ESP_LOGD(tag, "timings: %s", timings->toString().c_str());
//			FreeRTOS::sleep(1000);
//		}
//
//		printf("Tests done\n");
//		return;
//	}
//};
//
//static CurlTestTask *curlTestTask;

//class MyWiFiEventHandler: public WiFiEventHandler {
//
//	esp_err_t staGotIp(system_event_sta_got_ip_t event_sta_got_ip) {
//		ESP_LOGD(tag, "MyWiFiEventHandler(Class): staGotIp");
//
//		curlTestTask = new CurlTestTask();
//		curlTestTask->setStackSize(12000);
//		curlTestTask->start();
//
//		return ESP_OK;
//	}
//
//	esp_err_t staDisconnected(system_event_sta_disconnected_t info) {
//		ESP_LOGD(tag, "DISCONNECTED");
//		esp_restart();
//
//		return ESP_OK;
//	}
//};

void Client(void);

void app_main(void) {
//	ESP_LOGD(tag, "app_main: libcurl starting");
//	MyWiFiEventHandler *eventHandler = new MyWiFiEventHandler();
//
//	wifi = new WiFi();
//	wifi->setWifiEventHandler(eventHandler);
//
//	wifi->connectAP("WIFI SSID", "WIFI PASSWORD");
	Client();
}
