#ifndef HTTP_H_
#define HTTP_H_

//#define WEB_SERVER "192.168.0.100"
#define WEB_SERVER "172.20.10.3"
//#define WEB_SERVER "10.130.81.54"
#define WEB_PORT "3000"
#define WEB_URL "/measurements"

//#define WIFI_SSID "Daniel's iPhone"
//#define WIFI_PASSWORD "daniel'sPASSWORD!?"

//#define WIFI_SSID "ATG-Group1"
//#define WIFI_PASSWORD "PASSWORD"

 void http_send_request(const char* req);

 #endif
