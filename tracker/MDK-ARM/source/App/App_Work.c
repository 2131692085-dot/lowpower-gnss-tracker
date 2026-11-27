#include "App_Work.h"
#include "Int_LLCC68.h"
#include "Link_Config.h"
#include "usart.h"
#include <stdlib.h>
#include <ctype.h>

static int digits(const char *s, unsigned count)
{
    while (count--) {
        if (!isdigit((unsigned char)*s++)) return 0;
    }
    return 1;
}

static int coordinate(const char *s, char hemisphere, int latitude, double *out)
{
    unsigned width = latitude ? 4 : 5;
    unsigned limit = latitude ? 90 : 180;
    if (!digits(s, width)) return 0;
    const char *tail = s + width;
    if (*tail == '.') {
        if (!isdigit((unsigned char)*++tail)) return 0;
        while (isdigit((unsigned char)*tail)) ++tail;
    }
    if (*tail || !(hemisphere == (latitude ? 'N' : 'E') ||
                   hemisphere == (latitude ? 'S' : 'W'))) return 0;
    double raw = atof(s);
    unsigned deg = (unsigned)raw / 100;
    double minutes = raw - 100 * deg;
    if (minutes >= 60 || deg > limit || (deg == limit && minutes > 0)) return 0;
    *out = (deg + minutes / 60.0) * ((hemisphere == 'S' || hemisphere == 'W') ? -1 : 1);
    return 1;
}

/* line starts after '$', without CR/LF. Preserve empty NMEA fields. */
static int parse_rmc(char *line, Location_Value *v)
{
    char *star = strchr(line, '*');
    unsigned char sum = 0;
    char *fields[10] = {line};
    if (!star || strlen(star + 1) != 2 || !isxdigit((unsigned char)star[1]) ||
        !isxdigit((unsigned char)star[2])) return 0;
    for (char *p = line; p < star; ++p) sum ^= (unsigned char)*p;
    if (sum != strtoul(star + 1, NULL, 16)) return 0;
    *star = 0;
    for (int i = 1; i < 10; ++i) {
        char *comma = strchr(fields[i-1], ',');
        if (!comma) return 0;
        *comma = 0;
        fields[i] = comma + 1;
    }
    char *date = fields[9], *time = fields[1];
    char *end = strchr(date, ',');
    if (end) *end = 0;
    if (strlen(fields[0]) != 5 || strcmp(fields[0]+2, "RMC") ||
        strcmp(fields[2], "A") || strlen(date) != 6 || !digits(date, 6) ||
        !digits(time, 6) || strlen(fields[4]) != 1 || strlen(fields[6]) != 1 ||
        !coordinate(fields[3], fields[4][0], 1, &v->lat) ||
        !coordinate(fields[5], fields[6][0], 0, &v->lon)) return 0;
    snprintf((char *)v->date, sizeof(v->date), "20%.2s-%.2s-%.2s %.2s:%.2s:%.2s",
             date+4, date+2, date, time, time+2, time+4);
    return 1;
}

static int read_fix(Location_Value *v)
{
    char line[128];
    size_t used = 0;
    int collecting = 0;
    uint32_t start = HAL_GetTick();
    while (HAL_GetTick() - start < GPS_FIX_TIMEOUT_MS) {
        uint8_t ch;
        if (HAL_UART_Receive(&huart2, &ch, 1, 100) != HAL_OK) continue;
        if (ch == '$') {
            used = 0;
            collecting = 1;
        } else if (collecting && (ch == '\r' || ch == '\n')) {
            line[used] = 0;
            if (parse_rmc(line, v)) return 1;
            collecting = 0;
        } else if (collecting) {
            if (used < sizeof(line)-1) line[used++] = (char)ch;
            else collecting = 0;
        }
    }
    return 0;
}

void App_Work_Init(void)
{
    Int_DS3553_Init();
    Int_AT6558R_Init();
}

void App_Work_Execute(void)
{
    Location_Value fix = {0};
    char payload[192];
    if (!read_fix(&fix)) {
        puts("GPS: no valid fix, skipping report");
        return;
    }
    char steps[12] = "null";
    if (Int_DS3553_GetStepCount(&fix.step)) {
        snprintf(steps, sizeof(steps), "%lu", (unsigned long)fix.step);
    }
    int length = snprintf(payload, sizeof(payload),
        "{\"id\":\"%08lX%08lX%08lX\",\"lon\":%.6f,\"lat\":%.6f,\"date\":\"%s\",\"step\":%s}\n",
        (unsigned long)HAL_GetUIDw0(), (unsigned long)HAL_GetUIDw1(), (unsigned long)HAL_GetUIDw2(),
        fix.lon, fix.lat, (char *)fix.date, steps);
    if (length <= 0 || length >= sizeof(payload)) return;
    if (CLOUD_PORT && Int_QS100_SendData((uint8_t *)payload)) {
        puts("NB-IoT: TCP acknowledged");
        return;
    }
    if (Int_LLCC68_Init() != 0) {
        puts("LoRa: init failed");
        return;
    }
    puts(Int_LLCC68_Send((uint8_t *)payload, length) == 0 ? "LoRa: transmitted" : "LoRa: failed");
    Int_LLCC68_DeInit();
}
