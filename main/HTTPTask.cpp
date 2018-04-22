/*
 * HTTPTask.cpp
 *
 *  Created on: Apr 22, 2018
 *      Author: danielrosato
 */

#include <curl/curl.h>
#include <esp_log.h>
#include <RESTClient.h>
#include <string>
#include <Task.h>
#include <WiFi.h>
#include <WiFiEventHandler.h>
#include "Engine.h"
#include "sdkconfig.h"
#include "HTTPTask.h"

namespace receiver {

static char tag[] = "HTTPTask";

HTTPTask::HTTPTask() {
	// TODO Auto-generated constructor stub

}

HTTPTask::~HTTPTask() {
	// TODO Auto-generated destructor stub
}

void HTTPTask::run(void *data) {
	ESP_LOGD(tag, "Testing curl ...");
	RESTClient client;

	/**
	 * Test POST
	 */

	while(true) {
		RESTTimings *timings = client.getTimings();

		client.setURL("http://172.20.10.3:3000");
		client.addHeader("Content-Type", "application/json");
		client.post("hello world!");
		ESP_LOGD(tag, "Result: %s", client.getResponse().c_str());
		timings->refresh();
		ESP_LOGD(tag, "timings: %s", timings->toString().c_str());
		FreeRTOS::sleep(1000);
	}

	printf("Tests done\n");
	return;
}

} /* namespace receiver */
