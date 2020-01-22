#ifndef _GEODATETIME_H_
#define _GEODATETIME_H_

#include <Wire.h>
#include <RtcDS3231.h>
#include <TinyGPS++.h>
#include <time.h>

class GeoDateTime
{
public:
    virtual void set_time(time_t t) = 0;
};

#define GPS_INVALID_MSK 0x00UL
#define GPS_OUTDATE_MSK 0x01UL
#define GPS_COMMOFF_MSK 0x02UL
#define GPS_OK_MSK      0x03UL
#define GPSBaud         9600UL

struct GPSData_st
{
    static void set_location(bool valid, double lat, 
        double lng, double alt)
    {
        _lat = valid ? lat : -99.99;
        _lng = valid ? lng : -99.99;
        _alt = valid ? alt : -99.99;
    }
    static double _lat, _lng, _alt;
};
extern GPSData_st sysGPS;

class GPSControl : public GeoDateTime
{
private:
    TinyGPSPlus _tinygps;
    static time_t mk_timestamp(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec);
    
public:
    uint8_t readGPS(Stream* _serial, int msecs_wt);
    virtual void set_time(time_t t);
};

#define I2C Wire

#define DS3231
#define SOFTWARE_WIRE_SDA SDA  // Or whatever other pin you use
#define SOFTWARE_WIRE_SCL SCL  // Or whatever other pin you use

#define MY_TIMEZONE -3
#define MY_TIMEZONE_IN_SECONDS (MY_TIMEZONE * ONE_HOUR)

class RTCControl : public GeoDateTime
{
public:
    RTCControl() : Rtc(I2C)    {}
    RtcDS3231<TwoWire> Rtc;
    virtual void set_time(time_t t);
};

#endif
