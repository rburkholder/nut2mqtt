# nut2mqtt
Communication between Network UPS Tools (NUT) and MQTT

Uses [Network UPS Tools (NUT)](https://networkupstools.org/docs/developer-guide.chunked/ar01s08.html) library

Features:
* uses username/password, as created in /etc/nut/upsd.users, to login to the nut-server
* will enumerate all UPS instances maintained by the service
* will enumerate all available read-only variables, read-write variables, and commands for each instance
* on startup, will transmit all read-only variables as a JSON map to the specified MQTT topic
* on regular intervals, as specified in the nut2mqtt.cfg file, will transmit the variables designated as 'publish' as a JSON map to the specified MQTT topic
* the variables marked as 'numeric' in the nutqmqtt.cfg file will be transmited as numeric (rather than a string)
* on a broken connection to the MQTT server, a reconnections will be attempted (to be tested)

Packages to be installed (all platforms):
```
sudo apt install libnutclient-dev
sudo apt install libpaho-mqtt-dev
```

Packages (beaglebone) simply use the 'debian generic' section below, the other two sections are here as a reference:
```
# debian version 12.2 (not available yet)
sudo apt install libboost-filesystem1.81-dev
sudo apt install libboost-log1.81-dev
sudo apt install libboost-program-options1.81-dev
sudo apt install libboost-thread1.81-dev
# debian version 11.7 (as of 2023/10/08)
sudo apt install libboost-filesystem1.74-dev
sudo apt install libboost-log1.74-dev
sudo apt install libboost-program-options1.74-dev
sudo apt install libboost-thread1.74-dev
# debian generic
sudo apt install libboost-filesystem-dev
sudo apt install libboost-log-dev
sudo apt install libboost-program-options-dev
sudo apt install libboost-thread-dev
```

NOTE: my notes need revamping, but bottom line, you'll need to clone, build and install https://github.com/rburkholder/repertory first to obtain the MQTT interface.  I factored it out as I found I use it 
across a number of my projects. 

To Build statically linked application:
```
git clone https://github.com/rburkholder/nut2mqtt.git
cd nut2mqtt
mkdir build
cd build
cmake ..
make
cd ..
ln -s build/src/nut2mqtt nut2mqtt
# create & populate nut2mqtt.cfg with example
# then run:
./nut2mqtt
```

Nut packages required for running:

```
sudo apt install nut-client
sudo apt install nut-server

sed -i 's/MODE=none/MODE=standalone/' /etc/nut/nut.conf
sed -i 's/maxretry = 3/#maxretry = 4/' /etc/nut/ups.conf

# add to /etc/nut/upsd.users:

[admin]
  password = <password>
  actions = SET
  instcmds = ALL
#
[upsmon]
  password = <password>
  upsmon master

# example addition for APS UPS to /etc/nut/ups.conf:

[sm1500]
  driver = usbhid-ups
  port = auto
  desc = "sm1500"
  productid = 0002
  serial = <serial#>

# add to /etc/nut/upsmon.conf:

MONITOR sm1500@localhost 1 upsmon driver primary

# restart with settings:

sudo systemctl restart nut.target

# on an error like:  'libusb1: Could not open any HID devices: insufficient permissions on everything'
# add a file: /etc/udev/rules.d/90-nut-ups.rules
# with
ACTION=="add", SUBSYSTEM=="usb", ATTR{idVendor}=="051d", ATTR{idProduct}=="0003", MODE="0660", GROUP="nut", TEST==”power/control”, ATTR={power/control}=”on”
# disconnect the UPS cable and reconnect to reset the privileges

```

An example systemd control file can be found in systemd/nut2mqtt.service.  Change working directory & executable to suit.

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
numeric = output.powerfactor
numeric = output.frequency.nominal
numeric = output.voltage.nominal
numeric = ups.load
numeric = ups.delay.start
numeric = ups.delay.shutdown
numeric = ups.efficiency
numeric = ups.load.high
numeric = ups.power
numeric = ups.power.nominal
numeric = ups.realpower
numeric = ups.realpower.nominal
numeric = driver.parameter.pollfreq
numeric = driver.parameter.pollinterval
```

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
