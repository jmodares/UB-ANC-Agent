Copyright © 2014 - 2018 Jalil Modares

This program was part of my Ph.D. Dissertation research in the Department of Electrical Engineering at the University at Buffalo. I worked in UB's Multimedia Communications and Systems Laboratory with my Ph.D. adviser, [Prof. Nicholas Mastronarde](http://www.eng.buffalo.edu/~nmastron).

If you use this program for your work/research, please cite:
[J. Modares, N. Mastronarde, "UB-ANC: a Flexible Airborne Networking and Communications Testbed"](https://doi.org/10.1145/2980159.2980176).

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.

# UB-ANC Agent
## The mission software template for UB-ANC Drone
**UB-ANC Drone** is an open software/hardware research platform that combines aerial vehicles capable of autonomous flight with (i) sophisticated command and control capabilities and (ii) the ability to experiment with common wireless standards, such as IEEE 802.11 (Wi-Fi) and IEEE 802.15.4 (Zigbee), or custom protocols implemented on software-defined radios. UB-ANC Drone is designed with emphasis on modularity and extensibility, and is built around popular open-source projects and standards developed by the research and hobby communities. This makes UB-ANC Drone highly customizable, while also simplifying its adoption.

The **UB-ANC Agent**, which is the software that controls the drone, is designed to be compatible with any flight controller that supports the popular Micro Air Vehicle Communications Protocol, [MAVLink](http://mavlink.org). With its modular design, UB-ANC Drone provides tools for networking researchers to study airborne networking protocols and robotics researchers to study mission planning algorithms without worrying about other implementation details. Furthermore, we envision that it will facilitate collaborative work between networking and robotics researchers interested in problems related to network topology control and managing trade offs between mission objectives and network performance.

## Build
The current version of UB-ANC Agent uses [QGroundControl 3.2](http://qgroundcontrol.com) as its main library. The build process explained here is targeted for 64-bit Linux (Debian compatible) platforms. We recommend using [64-bit Ubuntu 16.04](http://releases.ubuntu.com/16.04/). Before building the UB-ANC Agent, run the following commands to setup your system:
```
sudo apt-get update && sudo apt-get upgrade
sudo apt-get install qt5-default qtbase5-dev \
    qtdeclarative5-dev qtpositioning5-dev \
    qtlocation5-dev libqt5svg5-dev libqt5serialport5-dev
```

Then, we can use `qmake` to build the agent:
```
cd ~
mkdir ub-anc && cd ub-anc
git clone https://github.com/jmodares/UB-ANC-Agent
mkdir build-agent && cd build-agent
qmake ../UB-ANC-Agent
make
```

The build process will take some time.

## Running in the UB-ANC Emulator
Before deploying any mission on an actual drone, we recommend that you first test it in the [UB-ANC Emulator](https://github.com/jmodares/UB-ANC-Emulator).

To build a mission for the UB-ANC Emulator, you can install qmake as described above, or you can source `setup_emulator.sh` (located in **~/ub-anc/emulator**):
```
source setup_emulator.sh
```

To test the UB-ANC Agent mission in the UB-ANC Emulator, simply copy the UB-ANC Agent executable `agent` (located in **~/ub-anc/build-agent/agent/release/**) into the emulator's **mav** directory:
```
cp ~/ub-anc/build-agent/agent/release/agent ~/ub-anc/emulator/mav/agent
```

Now, let's test the mission with 3 drones:
```
cd ~/ub-anc/emulator
./setup_objects.sh 3
./start_emulator.sh
```

This will launch the emulator using QGroundControl as the GUI. Note that you cannot start the mission until you receive the following messages from the drones (which are accessible by clicking on the [Vehicle Messages](https://docs.qgroundcontrol.com/en/toolbar/toolbar.html) status icon in QGroundControl):
```
[XXX] Info: EKF2 IMU0 is using GPS
[XXX] Info: EKF2 IMU1 is using GPS
```

Once you observe these messages, select the vehicles one-by-one from the *vehicle menu* and change their modes from **Stabilize** to **Guided** using the *flight mode* menu. After all the vehicles are in Guided mode, select *Vehicle 1*. Click **Disarmed** to arm Vehicle 1. You will be prompted to confirm that you want to arm the vehicle. After confirming, *Vehicle 1*'s status will change to **Armed** and it will takeoff to 5 m, fly east 5 m, hover for 20 seconds, and then land. When it first starts to hover, it will send a message to *Vehicle 2* that will trigger it to run the same mission. *Vehicle 2* will in turn trigger *Vehicle 3*. After *Vehicle 3* lands, the mission is over.

**Exercise 1:** Without quitting the UB-ANC Emulator, have the agents repeat the mission. *Hint:* Reset the Vehicles to **Guided** mode and arm *Vehicle 1* again.

**Exercise 2:** Relaunch the emulator to run the mission with **5** drones.
