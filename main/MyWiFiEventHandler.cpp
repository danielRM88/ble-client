/*
 * MyWiFiEventHandler.cpp
 *
 *  Created on: Apr 22, 2018
 *      Author: danielrosato
 */

#include "MyWiFiEventHandler.h"
#include "sdkconfig.h"
#include <string>
#include <esp_log.h>
#include "HTTPTask.h"
#include "Engine.h"

namespace receiver {

class Engine;
static char tag[] = "MyWiFiEventHandler";
Engine *engine;
HTTPTask *task;

MyWiFiEventHandler::MyWiFiEventHandler(Engine *e, HTTPTask *t) {
	// TODO Auto-generated constructor stub
	engine = e;
	task = t;
}

MyWiFiEventHandler::~MyWiFiEventHandler() {
	// TODO Auto-generated destructor stub
}

esp_err_t MyWiFiEventHandler::staGotIp(system_event_sta_got_ip_t event_sta_got_ip) {
	ESP_LOGD(tag, "MyWiFiEventHandler(Class): staGotIp");

//	HTTPTask *curlTestTask = new HTTPTask();
//	task->setStackSize(12000);
	task->start();

	return ESP_OK;
}

esp_err_t MyWiFiEventHandler::staDisconnected(system_event_sta_disconnected_t info) {
	ESP_LOGD(tag, "DISCONNECTED");
//		esp_restart();
//	engine = new Engine();
	engine->start();

	return ESP_OK;
}

} /* namespace receiver */
