/*
 * MyWiFiEventHandler.h
 *
 *  Created on: Apr 22, 2018
 *      Author: danielrosato
 */
#include <WiFiEventHandler.h>
#include "HTTPTask.h"
#include "Engine.h"

#ifndef MAIN_MYWIFIEVENTHANDLER_H_
#define MAIN_MYWIFIEVENTHANDLER_H_

namespace receiver {

class MyWiFiEventHandler : public WiFiEventHandler {
public:
	MyWiFiEventHandler(Engine *e, HTTPTask *t);
	virtual ~MyWiFiEventHandler();
	esp_err_t staGotIp(system_event_sta_got_ip_t event_sta_got_ip);
	esp_err_t staDisconnected(system_event_sta_disconnected_t info);
};

} /* namespace receiver */

#endif /* MAIN_MYWIFIEVENTHANDLER_H_ */
