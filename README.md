# nut2mqtt
Communication between Network UPS Tools (NUT) and MQTT

Uses [Network UPS Tools (NUT)](https://networkupstools.org/docs/developer-guide.chunked/ar01s08.html) library

```
$ cat x64/debug/nut2mqtt.cfg
nut_host = localhost
nut_username = admin
nut_password = password
nut_poll_interval = 15

mqtt_id = nut1
mqtt_host = 127.0.0.1
mqtt_username = admin
mqtt_password = password
mqtt_topic = nut
```

https://github.com/dniklewicz/ups-mqtt
https://github.com/jnovack/nut-to-mqtt

maybe track the following metrics from NUT:

    battery.charge - Battery charge (percent of full)
    battery.runtime - Battery runtime (seconds)
    battery.voltage - Battery voltage (V)
    input.voltage - Input voltage (V)
    ups.load - Load on UPS (percent of full)
    ups.status - UPS status

d
From some user on the internet:  the poll interval in UPS.CONF controls the rate at which NUT queries the UPS, this is where I had my problem with my CyberPower units, the default value of 2 seconds caused my UPS to quit talking to NUT over the USB connection. I ended up with a value of 15 seconds and it has been solid for over a year of continuous operation. I did not try to tune it down to a smaller value, so I do not know what the minimum is for the CyberPower.
