#ifndef LINK_CONFIG_H
#define LINK_CONFIG_H
/* Enter the IPv4 printed by NetLab as four decimal octets. Port 0 disables NB-IoT. */
#define CLOUD_IPV4 0,0,0,0
#define CLOUD_PORT 0
#define LAN_SERVER_IPV4 192,168,45,20
#define LAN_SERVER_PORT 777
#define GATEWAY_IPV4 192,168,45,120
#define ROUTER_IPV4 192,168,45,1
#define SUBNET_MASK 255,255,255,0
/* 1 forwards gateway traffic to the same public listener as the tracker. */
#define GATEWAY_USE_CLOUD 0
#if GATEWAY_USE_CLOUD && CLOUD_PORT == 0
#error Configure CLOUD_PORT before routing gateway traffic to the cloud
#endif
#define REPORT_INTERVAL_SECONDS 30U
#define GPS_FIX_TIMEOUT_MS 120000U
#endif
