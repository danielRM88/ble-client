/*
 * Engine.cpp
 *
 *  Created on: Apr 22, 2018
 *      Author: danielrosato
 */

#include "Engine.h"
#include <WiFi.h>
#include "MyWiFiEventHandler.h"

namespace receiver {

class MyWiFiEventHandler;
HTTPTask *task;

Engine::Engine() {
	// TODO Auto-generated constructor stub
	task = new HTTPTask();
	task->setStackSize(12000);
}

Engine::~Engine() {
	// TODO Auto-generated destructor stub
}

void Engine::start() {
//	Engine *engine = new Engine();
	MyWiFiEventHandler *eventHandler = new MyWiFiEventHandler(this, task);

	WiFi *wifi = new WiFi();
	wifi->setWifiEventHandler(eventHandler);

	wifi->connectAP("Daniel's iPhone", "daniel'sPASSWORD!?");
}

} /* namespace receiver */
