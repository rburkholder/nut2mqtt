# nut2mqtt
Communication between Network UPS Tools (NUT) and MQTT

Uses [Network UPS Tools (NUT)](https://networkupstools.org/docs/developer-guide.chunked/ar01s08.html) library

Packages:
```
sudo apt install libnutclient-dev
sudo apt install libpaho-mqtt-dev
```

Configuration File:
```
$ cat x64/debug/nut2mqtt.cfg
nut_enumerate = true
nut_host = localhost
nut_username = admin
nut_password = password
nut_poll_interval = 15

nut_publish = battery.charge
nut_publish = battery.voltage
nut_publish = battery.runtime
nut_publish = battery.temperature
nut_publish = input.voltage
nut_publish = output.current
nut_publish = output.frequency
nut_publish = output.voltage
nut_publish = ups.load
nut_publish = ups.status

nut_numeric = battery.charge
nut_numeric = battery.charge.low
nut_numeric = battery.charge.warning
nut_numeric = battery.runtime
nut_numeric = battery.runtime.low
nut_numeric = battery.temperature
nut_numeric = battery.voltage
nut_numeric = battery.voltage.nominal
nut_numeric = input.voltage
nut_numeric = input.transfer.high
nut_numeric = input.transfer.low
nut_numeric = input.voltage.nominal
nut_numeric = output.current
nut_numeric = output.frequency
nut_numeric = output.voltage
nut_numeric = output.voltage.nominal
nut_numeric = ups.load
nut_numeric = ups.delay.start
nut_numeric = ups.delay.shutdown
nut_numeric = ups.realpower.nominal
nut_numeric = driver.parameter.pollfreq
nut_numeric = driver.parameter.pollinterval

mqtt_id = nut1
mqtt_host = 127.0.0.1
mqtt_username = admin
mqtt_password = password
mqtt_topic = nut
```

Sample Output
```
nut/sm1500 {"battery.charge":100,"battery.runtime":3099,"battery.voltage":26.3,"ups.status":"OL"}
nut/sm1500 {"battery.charge":100,"battery.runtime":3202,"battery.voltage":26.3,"ups.status":"OB DISCHRG"}
nut/sm1500 {"battery.charge":89,"battery.runtime":2983,"battery.voltage":24.4,"ups.status":"OB DISCHRG"}
nut/sm1500 {"battery.charge":90,"battery.runtime":3077,"battery.voltage":24.5,"ups.status":"OL CHRG"}
nut/sm1500 {"battery.charge":90,"battery.runtime":3077,"battery.voltage":26.1,"ups.status":"OL CHRG"}
```

maybe track the following metrics from NUT:

    battery.charge - Battery charge (percent of full)
    battery.runtime - Battery runtime (seconds)
    battery.voltage - Battery voltage (V)
    input.voltage - Input voltage (V)
    ups.load - Load on UPS (percent of full)
    ups.status - UPS status

From some user on the internet:  the poll interval in UPS.CONF controls the rate at which NUT queries the UPS, this is where I had my problem with my CyberPower units, the default value of 2 seconds caused my UPS to quit talking to NUT over the USB connection. I ended up with a value of 15 seconds and it has been solid for over a year of continuous operation. I did not try to tune it down to a smaller value, so I do not know what the minimum is for the CyberPower.

Inspiration:

* https://github.com/dniklewicz/ups-mqtt
* https://github.com/jnovack/nut-to-mqtt
* https://networkupstools.org/docs/developer-guide.chunked/ar01s08.html
