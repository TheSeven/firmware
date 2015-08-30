#include "global.h"
#include "app/main.h"
#include "sys/time.h"
#include "sys/util.h"
#include "common.h"
#include "rtc.h"
#include "radio.h"
#include "sensor.h"
#include "storage.h"


void SENSORNODE_CORE_OPTIMIZE startMeasurement()
{
    measurementId++;
    liveData.measId = measurementId;
    storageDriver->stayAwake(true);
    makeSpaceForSensorMeta();  // Do power-hungry erase here, to catch low battery conditions before writing meta entry
    writeMeta(0, 0, 0);
    storageDriver->stayAwake(false);
    rtcDriver->reset();
    for (int i = 0; i < sensorCount; i++)
    {
        sensors[i]->dataOffset = 0;
        sensors[i]->writePtr = 0;
        sensors[i]->nextTime = 0;
    }
    deepSleep = false;
}


void SENSORNODE_CORE_OPTIMIZE updateLiveDataSize()
{
    liveDataSize = 0;
    for (int i = 0; i < sensorCount; i++)
    {
        if (!sensors[i]->meta.attr.interval) continue;
        uint32_t offset = sensors[i]->meta.offset;
        uint32_t len = sensors[i]->bytesPerPoint;
        if (offset + len > liveDataSize && offset + len <= sizeof(liveData.sensorData)) liveDataSize = offset + len;
    }
    memset(liveData.sensorData, 0xff, liveDataSize);
    radioDriver->beacon->payloadLen = liveDataSize ? liveDataSize + 4 : 0;
}


int SENSORNODE_CORE_OPTIMIZE main()
{
    sensornode_sysinit();
    initStorage();
    sensornode_init_sensors();
    updateLiveDataSize();
    startMeasurement();

    while (true)
    {
        now = rtcDriver->getTime();
        deepSleep = true;

        handleRadio();

        bool first = true;
        int nextSensor = now + 65536;
        for (int i = 0; i < sensorCount; i++)
        {
            if (!sensors[i]->meta.attr.interval) continue;
            if (!TIME_AFTER(sensors[i]->nextTime, now))
            {
                if (first)
                {
                    first = false;
                    storageDriver->stayAwake(true);
                    sensornode_enable_sensors();
                }
                uint32_t value = sensors[i]->readValue();
                int offset = sensors[i]->meta.offset;
                int len = sensors[i]->bytesPerPoint;
                if (offset + len <= liveDataSize)
                {
                    memcpy(liveData.sensorData + offset, &value, len);
                    measurementTime = sensors[i]->nextTime;
                    liveData.boardTime = measurementTime;
                }
                sensors[i]->nextTime += sensors[i]->meta.attr.interval;
                writeDataPoint(i, value);
            }
            if (sensors[i]->nextTime < nextSensor) nextSensor = sensors[i]->nextTime;
        }
        if (!first)
        {
            sensornode_disable_sensors();
            storageDriver->stayAwake(false);
        }

        enter_critical_section();
        if (radioDriver->detectIRQ()) continue;
        if (deepSleep)
        {
            int until = commTimeout;
            if (TIME_AFTER(until, nextBeacon)) until = nextBeacon;
            if (TIME_AFTER(until, nextSensor)) until = nextSensor;
            if (!TIME_AFTER(until, now)) until = now + 1;
            radioDriver->sleep();
            rtcDriver->sleepUntil(until);
            radioDriver->wake();
        }
        else idle();
        leave_critical_section();
    }
}
