# nut2mqtt
Communication between Network UPS Tools (NUT) and MQTT

Uses [Network UPS Tools (NUT)](https://networkupstools.org/docs/developer-guide.chunked/ar01s08.html) library

Packages:
```
sudo apt install libnutclient-dev
sudo apt install libpaho-mqtt-dev
```

Note: I use my own [boost building script](https://github.com/rburkholder/libs-build).  Your distirbution's
boost packages should work as well.  You'll need to adjust the Boost version in the src/CMakeLists.txt
file to reflect the version.

To Build:
```
mkdir build
cd build
cmake ..
make
src/nut2mqtt
```

Configuration File template (change usernames, passwords, and addresses):
```
$ cat x64/debug/nut2mqtt.cfg
nut_enumerate = true    # emit found variables & commands on first iteration
nut_host = localhost
nut_username = admin
nut_password = password
nut_poll_interval = 15

mqtt_id = nut1
mqtt_host = 127.0.0.1
mqtt_username = admin
mqtt_password = password
mqtt_topic = nut

publish = battery.charge
publish = battery.voltage
publish = battery.runtime
publish = battery.temperature
publish = input.voltage
publish = output.current
publish = output.frequency
publish = output.voltage
publish = ups.load
publish = ups.status

numeric = battery.charge
numeric = battery.charge.low
numeric = battery.charge.warning
numeric = battery.runtime
numeric = battery.runtime.low
numeric = battery.temperature
numeric = battery.voltage
numeric = battery.voltage.nominal
numeric = input.voltage
numeric = input.transfer.high
numeric = input.transfer.low
numeric = input.voltage.nominal
numeric = output.current
numeric = output.frequency
numeric = output.voltage
numeric = output.voltage.nominal
numeric = ups.load
numeric = ups.delay.start
numeric = ups.delay.shutdown
numeric = ups.realpower.nominal
numeric = driver.parameter.pollfreq
numeric = driver.parameter.pollinterval
```

ToDo:
* reconnect nut client on disconnect

Sample Topic/Message Output:
```
nut/sm1500 {"battery.charge":100,"battery.runtime":3099,"battery.voltage":26.3,"ups.status":"OL"}
nut/sm1500 {"battery.charge":100,"battery.runtime":3202,"battery.voltage":26.3,"ups.status":"OB DISCHRG"}
nut/sm1500 {"battery.charge":89,"battery.runtime":2983,"battery.voltage":24.4,"ups.status":"OB DISCHRG"}
nut/sm1500 {"battery.charge":90,"battery.runtime":3077,"battery.voltage":24.5,"ups.status":"OL CHRG"}
nut/sm1500 {"battery.charge":90,"battery.runtime":3077,"battery.voltage":26.1,"ups.status":"OL CHRG"}
```

From some user on the internet:  the poll interval in UPS.CONF controls the rate at which NUT queries the UPS, this is where I had my problem with my CyberPower units, the default value of 2 seconds caused my UPS to quit talking to NUT over the USB connection. I ended up with a value of 15 seconds and it has been solid for over a year of continuous operation. I did not try to tune it down to a smaller value, so I do not know what the minimum is for the CyberPower.

Inspiration:

* https://github.com/dniklewicz/ups-mqtt
* https://github.com/jnovack/nut-to-mqtt
* https://networkupstools.org/docs/developer-guide.chunked/ar01s08.html
