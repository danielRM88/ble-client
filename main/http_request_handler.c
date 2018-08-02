
#include "http_request_handler.h"

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"


/* The event group allows multiple bits for each event,
   but we only care about one event - are we connected
   to the AP with an IP? */
const int CONNECTED_BIT = BIT0;
static const char *TAG = "HttpRequest";

void http_send_request(const char* req)
{
	const struct addrinfo hints = {
		.ai_family = AF_INET,
		.ai_socktype = SOCK_STREAM,
	};
	struct addrinfo *res;
	struct in_addr *addr;
	int s;
	ESP_LOGI(TAG, "Connected to AP");

	int err = getaddrinfo(WEB_SERVER, WEB_PORT, &hints, &res);

	if(err != 0 || res == NULL) {
		ESP_LOGE(TAG, "DNS lookup failed err=%d res=%p", err, res);
		return;
	}

	addr = &((struct sockaddr_in *)res->ai_addr)->sin_addr;
	ESP_LOGI(TAG, "DNS lookup succeeded. IP=%s", inet_ntoa(*addr));

	s = socket(res->ai_family, res->ai_socktype, 0);
	ESP_LOGI(TAG, "socket=%d", s);
	if(s < 0) {
		ESP_LOGE(TAG, "... Failed to allocate socket.");
		freeaddrinfo(res);
		return;
	}
	ESP_LOGI(TAG, "... allocated socket");

	if(connect(s, res->ai_addr, res->ai_addrlen) != 0) {
		ESP_LOGE(TAG, "... socket connect failed errno=%d", errno);
		close(s);
		freeaddrinfo(res);
		return;
	}

	ESP_LOGI(TAG, "... connected");
	freeaddrinfo(res);

	if (write(s, req, strlen(req)) < 0) {
		ESP_LOGE(TAG, "... socket send failed");
		close(s);
		return;
	}
	ESP_LOGI(TAG, "... socket send success");

	struct timeval receiving_timeout;
	receiving_timeout.tv_sec = 5;
	receiving_timeout.tv_usec = 0;
	if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &receiving_timeout,
			sizeof(receiving_timeout)) < 0) {
		ESP_LOGE(TAG, "... failed to set socket receiving timeout");
		close(s);
		return;
	}
	ESP_LOGI(TAG, "... set socket receiving timeout success");
	close(s);
}
