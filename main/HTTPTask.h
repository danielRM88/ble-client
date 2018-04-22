/*
 * HTTPTask.h
 *
 *  Created on: Apr 22, 2018
 *      Author: danielrosato
 */

#include <curl/curl.h>
#include <esp_log.h>
#include <RESTClient.h>
#include <Task.h>

#ifndef MAIN_HTTPTASK_H_
#define MAIN_HTTPTASK_H_

namespace receiver {

class HTTPTask: public Task {
public:
	HTTPTask();
	virtual ~HTTPTask();
	void run(void *data);
};

} /* namespace receiver */

#endif /* MAIN_HTTPTASK_H_ */
