#ifndef HTTP_H_
#define HTTP_H_

#define WEB_SERVER "10.130.81.54"
#define WEB_PORT "4500"
#define WEB_URL "/measurements"

#define WIFI_SSID "WIFI_SSID"
#define WIFI_PASSWORD "WIFI_PASSWORD"

 void http_send_request(const char* req);

 #endif
