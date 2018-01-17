Copyright © 2014 - 2018 Jalil Modares

This program was part of my Ph.D. Dissertation research in the Department of Electrical Engineering at the University at Buffalo. I worked in UB's Multimedia Communications and Systems Laboratory with my Ph.D. adviser, [Prof. Nicholas Mastronarde](http://www.eng.buffalo.edu/~nmastron).

If you use this program for your work/research, please cite:
[J. Modares, N. Mastronarde, "UB-ANC: a Flexible Airborne Networking and Communications Testbed"](https://doi.org/10.1145/2980159.2980176).

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.

# UB-ANC Agent
## The mission software template for UB-ANC Drone
**UB-ANC Drone** is an open software/hardware platform that aims to facilitate rapid testing and repeatable comparative evaluation of airborne networking and communications protocols at different layers of the protocol stack. It combines quadcopters capable of autonomous flight with sophisticated command and control capabilities and embedded software-defined radios (SDRs), which enable flexible deployment of novel communications and networking protocols. This is in contrast to existing airborne network testbeds, which rely on standard inflexible wireless technologies, e.g., Wi-Fi or Zigbee. UB-ANC Drone is designed with emphasis on modularity and extensibility, and is built around popular open-source projects and standards developed by the research and hobby communities. This makes UB-ANC Drone highly customizable, while also simplifying its adoption. The **UB-ANC Agent**, which is the software that controls the drone, is designed to be compatible with any flight controller that supports the popular Micro Air Vehicle Communications Protocol, [MAVLink](http://mavlink.org). With its modular design, UB-ANC Drone provides tools for networking researchers to study airborne networking protocols and robotics researchers to study mission planning algorithms without worrying about other implementation details. Furthermore, we envision that it will facilitate collaborative work between networking and robotics researchers interested in problems related to network topology control and managing trade offs between mission objectives and network performance.

## Build
The current version of UB-ANC Agent uses [QGroundControl 3.2](http://qgroundcontrol.com) as its main library. The build process explained here is targeted for Linux (Debian compatible) platforms.
```
sudo apt-get update && sudo apt-get upgrade
sudo apt-get install qt5-default qtbase5-dev \
    qtdeclarative5-dev qtpositioning5-dev \
    qtlocation5-dev libqt5svg5-dev libqt5serialport5-dev
```

Then we can use `qmake` to build the agent.

```
cd ~
mkdir ub-anc && cd ub-anc
git clone https://github.com/jmodares/UB-ANC-Agent
mkdir build && cd build
qmake ../UB-ANC-Agent
```
