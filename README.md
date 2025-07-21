# Copyright

© 2016 Jean-Luc Deltombe LX3JL and Luc Engelmann LX1IQ

The XLX Multiprotocol Gateway Reflector Server is part of the software system
for the D-Star Network.
The sources are published under GPL Licenses.

# Supported Protocols since XLX v2.5.x

- In D-Star, Icom-G3Terminal, DExtra, DPLus and DCS
- In DMR, DMRPlus (dongle) and DMRMmdvm
- In C4FM, YSF, Wires-X and IMRS
- XLX Interlink protocol

# Usage

The packages which are described in this document are designed to install server
software which is used for the D-Star network infrastructure.
It requires a 24/7 internet connection which can support 20 voice streams or more
to connect repeaters and hotspot dongles!!

- The server requires a fix IP-address !
- The public IP address should have a DNS record which must be published in the
common host files.

If you want to run this software please make sure that you can provide this
service free of charge, like the developer team provides the software and the
network infrastructure free of charge!

## Command Line Arguments

XLX supports flexible network configuration through command line arguments:

```
xlxd callsign xlxdip/interface ambedip
```

Examples:
- `xlxd XLX999 192.168.178.212 127.0.0.1` (using IP address)
- `xlxd XLX999 eth0 127.0.0.1` (using network interface)
- `xlxd XLX999 wlan0 127.0.0.1` (using wireless interface)

**Note:** If the second parameter is not a valid IPv4 address, xlxd will automatically
attempt to resolve it as a network interface name and use the IPv4 address assigned
to that interface.

# Docker Installation (Recommended)

XLX can be easily deployed using Docker, which provides a consistent environment
and simplified deployment process.

## Building the Docker Image

```bash
# Clone the repository
git clone https://github.com/sctg-development/xlxd.git
cd xlxd

# Build the Docker image
docker build -t xlxd:latest .
```

## Running with Docker

```bash
# Run XLX in a Docker container
docker run -d \
  --name xlxd \
  --restart unless-stopped \
  -p 80:80 \
  -p 10001:10001/udp \
  -p 10002:10002/udp \
  -p 42000:42000/udp \
  -p 8880:8880/udp \
  -p 62030:62030/udp \
  -p 21110:21110/udp \
  xlxd:latest
```

## Docker Compose

Create a `docker-compose.yml` file:

```yaml
version: '3.8'
services:
  xlxd:
    build: .
    container_name: xlxd
    restart: unless-stopped
    ports:
      - "80:80"           # HTTP Dashboard
      - "10001:10001/udp" # JSON interface XLX Core
      - "10002:10002/udp" # XLX interlink
      - "42000:42000/udp" # YSF protocol
      - "8880:8880/udp"   # DMR+ DMO mode
      - "62030:62030/udp" # MMDVM protocol
      - "21110:21110/udp" # Yaesu IMRS protocol
    environment:
      - XLX_CALLSIGN=XLXKERBRA
      - XLX_LISTEN_IP=0.0.0.0
      - XLX_TRANSCODER_IP=127.0.0.1
```

Then run:
```bash
docker-compose up -d
```

## Docker Features

- **Multi-stage build**: Optimized image size with separate build and runtime stages
- **Supervisor**: Process management for xlxd, xlxecho, and Apache services
- **Automatic service management**: All services start automatically and restart on failure
- **Web dashboard**: Accessible on port 80
- **Logs**: Service logs available via `docker logs xlxd`

# Requirements

The software packages for Linux are tested on Debian7 (Wheezy) 32 and 64bit or newer.
Raspbian will work but is not recommended.
Please use the stable version listed above, we cannot support others.

# Installation

## Debian 7 (Wheezy) 32 and 64bit

###### After a clean installation of debian make sure to run update and upgrade
```
# apt-get update
# apt-get upgrade
```
###### Install Git
```
# apt-get install git git-core
```
###### Install webserver with PHP5 support
```
# apt-get install apache2 php5
```

###### Install g++ compiler
```
# apt-get install build-essential
# apt-get install g++-4.7 (skip this step on Debian 8.x) 
 
```
###### Download source code
```
# git clone https://github.com/LX3JL/xlxd.git
# cd xlxd/src/

```
###### If you want to enable YSF, edit the main.h file 
```
# nano main.h

Then modify the following line:
#define YSF_AUTOLINK_ENABLE 0 <- Replace 0 with 1

Review YSF Frequencies
#define YSF_DEFAULT_NODE_TX_FREQ        437000000   <-- in Hz
#define YSF_DEFAULT_NODE_RX_FREQ        437000000   <-- in Hz

```
###### Download and compile the XLX sources
```
# make clean
# make
# make install
```

**Note**: The compiled xlxd binary now supports automatic network interface resolution.
You can specify either an IP address or network interface name (e.g., eth0, wlan0)
as the second parameter when starting xlxd.

###### Copy startup script "xlxd" to /etc/init.d
```
# cp ~/xlxd/scripts/xlxd /etc/init.d/xlxd
```

###### Adapt the default configuration to your needs
```
# pico /xlxd/xlxd.config
```
###### Download the dmrid.dat from the XLXAPI server to your xlxd folder
```
# wget -O /xlxd/dmrid.dat http://xlxapi.rlx.lu/api/exportdmr.php
```

###### Check your FTDI driver and install the AMBE service according to the readme in AMBEd
```
 
```

###### Last step is to declare the service for automatic startup and shutdown
```
# update-rc.d xlxd defaults
```

###### Start or stop the service with
```
# service xlxd start
# service xlxd stop
```

###### Copy dashboard to /var/www
```
# cp -r ~/xlxd/dashboard /var/www/db
```

###### Give the dashboard read access to the server log file 
```
# chmod +r /var/log/messages 
```

###### Reboot server to see if the auto-start is working
```
# reboot
```

# Firewall settings #

XLX Server requires the following ports to be open and forwarded properly for in- and outgoing network traffic:

## Essential Ports (Required for Docker deployment)
 - TCP port 80            (HTTP Dashboard)
 - UDP port 10001         (JSON interface XLX Core)
 - UDP port 10002         (XLX interlink)
 - UDP port 42000         (YSF protocol)
 - UDP port 8880          (DMR+ DMO mode)
 - UDP port 62030         (MMDVM protocol)
 - UDP port 10100         (AMBE controller port)
 - UDP port 10101 - 10199 (AMBE transcoding port)
 - UDP port 12345 - 12346 (Icom Terminal presence and request port)
 - UDP port 40000         (Icom Terminal dv port)

**Note for Docker users**: The essential ports listed above are exposed by default in the Docker configuration.
Additional ports can be added to the Docker run command as needed.

# YSF Master Server

Pay attention, the XLX Server acts as an YSF Master, which provides 26 wires-x rooms.
It has nothing to do with the regular YSFReflector network, hence you don’t need to register your XLX at ysfreflector.de !
Nevertheless it is possible.

© 2016 Jean-Luc Deltombe (LX3JL) and Luc Engelmann (LX1IQ)
