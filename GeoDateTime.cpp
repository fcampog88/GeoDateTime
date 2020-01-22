#include "GeoDateTime.h"

double GPSData_st::_lat = -99.99;
double GPSData_st::_lng = -99.99;
double GPSData_st::_alt = -99.99;

uint8_t GPSControl::readGPS(Stream* _serial, int msecs_wt)
{
    String debug_str = "";
    
    while(!_serial->available())
    {
        delay(1);
        if(--msecs_wt <= 0) return GPS_COMMOFF_MSK;
    }
    while (_serial->available())
    {
        delay(1);
        msecs_wt = (--msecs_wt <= 0) ? 0 : msecs_wt;
        _tinygps.encode(_serial->read());
    }
    if(msecs_wt <= 0 && _tinygps.charsProcessed() < 10)  return GPS_COMMOFF_MSK;
    
    if(!_tinygps.location.isValid())
    {
        if (msecs_wt <= 0)  return GPS_INVALID_MSK;     
        return readGPS(_serial, msecs_wt);
    }
    
    if (_tinygps.location.age() > 1500 || _tinygps.date.age() > 1500)
    {
        return GPS_OUTDATE_MSK;
    }
    
    set_position(_tinygps.location.lat() * ONE_DEGREE, _tinygps.location.lng() * ONE_DEGREE);
    GPSData_st::set_location(true, _tinygps.location.lat(), _tinygps.location.lng(), _tinygps.altitude.meters());
    time_t t = mk_timestamp(_tinygps.date.year(), _tinygps.date.month(), _tinygps.date.day(), _tinygps.time.hour(), _tinygps.time.minute(), _tinygps.time.second());
    set_time(t);
    
    return GPS_OK_MSK;
}

time_t GPSControl::mk_timestamp(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec)
{
    tm _gps_dateTime;
    _gps_dateTime.tm_year  = year-1900;
    _gps_dateTime.tm_mon   = month-1;
    _gps_dateTime.tm_mday  = day;
    _gps_dateTime.tm_hour  = hour - 3;
    _gps_dateTime.tm_min   = min;
    _gps_dateTime.tm_sec   = sec;
    
    time_t t = mk_gmtime(&_gps_dateTime);
    return t;
}

void GPSControl::set_time(time_t t)
{
    if(t < time(nullptr)-ONE_HOUR || t > time(nullptr)+ONE_HOUR)    // One our difference
        set_system_time(t);
}

void RTCControl::set_time(time_t t)
{
    if(t < time(nullptr)-ONE_HOUR || t > time(nullptr)+ONE_HOUR)    // One our difference
    {
        Rtc.SetTime(&t);
        set_system_time(t);
    }
}
