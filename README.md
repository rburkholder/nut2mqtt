# nut2mqtt
Communication between Network UPS Tools (NUT) and MQTT

Uses [Network UPS Tools (NUT)](https://networkupstools.org/docs/developer-guide.chunked/ar01s08.html) library



Some possible configs:

      - MQTT_ID=nut
      - MQTT_PATH=nut
      - MQTT_HOST=mqtt://<ip address of mqtt broker>
      - MQTT_USERNAME=<mqtt username>
      - MQTT_PASSWORD=<mqtt password>
      - NUT_HOST=<ip address of nut server>
      - NUT_USERNAME=<nut username>
      - NUT_PASSWORD=<nut password>


https://github.com/dniklewicz/ups-mqtt
```
[UPS]
# Address of NUT server
#hostname=localhost

[MQTT]
# Base MQTT topic
#base_topic=home/ups

# Address of MQTT server
#hostname=localhost

# MQTT server port
#port=1883

# MQTT username
#username=

# MQTT password
#password=

[General]
# Polling interval in seconds
#interval=60
```

https://github.com/jnovack/nut-to-mqtt

$ nut-to-mqtt \
    -nut_hostname=192.168.9.10 -nut_username=monitor -nut_password=hunter2 \
    -mqtt_hostname=172.23.4.5 -mqtt_username=user24601 -mqtt_password=hunter2


Currently tracks the following metrics from NUT:

    battery.charge - Battery charge (percent of full)
    battery.runtime - Battery runtime (seconds)
    battery.voltage - Battery voltage (V)
    input.voltage - Input voltage (V)
    ups.load - Load on UPS (percent of full)
    ups.status - UPS status


The pollinterval in UPS.CONF controls the rate at which NUT queries the UPS, this is where I had my problem with my CyberPower units, the default value of 2 seconds caused my UPS to quit talking to NUT over the USB connection. I ended up with a value of 15 seconds and it has been solid for over a year of continuous operation. I did not try to tune it down to a smaller value, so I do not know what the minimum is for the CyberPower:    
