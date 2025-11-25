Cellular Network Simulator (OOPD Final Project)

This project is an **Object-Oriented C++ simulation framework** that models different generations of cellular communication technologies: **2G, 3G, 4G, and 5G**. It demonstrates multiple programming concepts such as **inheritance, polymorphism, templates, exception handling, dynamic memory, syscalls-based I/O**, and **threading comparison (simulated)**.

The project runs entirely using **low-level Linux syscalls**, without relying on C/C++ Standard Library I/O (`iostream`, `string`, `vector`, etc.), except for `<thread>` as allowed by the specification. All input/output is handled through a custom `basicIO` interface.

Features

Multiple Communication Standards

Each standard (2G, 3G, 4G, 5G) defines:

* Users per channel
* Messages per user
* Bandwidth allocation
* Channel size
* Antenna count (MIMO support)
* Overhead per 100 messages
* Speed (Mb/s)

Complete OOP Architecture

* `CommunicationStandard` (abstract base class)
* `TwoG`, `ThreeG`, `FourG`, `FiveG` (derived classes)
* `CellTower` manages users and channel allocation
* `CellularCore` models message processing & overhead
* `UserDevice` represents each client device

Templates

Two template components are included:

* `div_up<T>` (generic integer ceiling division)
* `SimpleHolder<T>` (lightweight template class to wrap values)
* `ChannelRange<T>` (wrapper to describe a range of user IDs using templates)

These demonstrate practical template usage **without violating the "no stdlib" rule**.

Exception Handling 

The project uses:

* `try/catch` blocks
* `throw` with string literals (`const char*`)

This allows exception handling **without** including `<exception>` or other standard headers.

Syscall-Based I/O (No std::cin/std::cout)

All I/O is implemented in:

```
include/basicIO.h
src/basicIO.cpp
```

Using raw Linux syscalls via `syscall.S`.

Multi-threading Comparison (Simulated)

The simulation compares:

* **Non-threaded time** (single core)
* **Threaded time** (based on `hardware_concurrency()`)
* **Per-thread message distribution**
* **Speedup factor**

Threads are **not actually created** — the tool uses mathematical modeling to satisfy the project requirement.

Fully Custom Build System

A handcrafted `Makefile` builds:

* `cellsim` (release, optimized)
* `cellsim_debug` (debug, uses `-g`)

Both versions use:

* `-nostartfiles`
* NASM-assembled syscall entry file (`syscall.S`)

---

Project Structure

```
OOPD-FINAL/
│
├── include/
│   ├── 2G.h
│   ├── 3G.h
│   ├── 4G.h
│   ├── 5G.h
│   ├── basicIO.h
│   ├── CellTower.h
│   ├── CellularCore.h
│   ├── CommunicationStandard.h
│   ├── UserDevice.h
│   ├── TemplateUtilities.h        ← template utilities
│   ├── ChannelRange.h             ← template class
│   └── Exception.h                ← custom exception class
│
├── src/
│   ├── basicIO.cpp
│   ├── main.cpp                   ← main simulator logic
│   ├── Exception.cpp              ← custom exception implementation
│   └── syscall.S                  ← raw Linux syscall layer
│
├── Makefile

```

---

 How The Simulator Works

## 1. User selects a communication standard

```
1 = 2G
2 = 3G
3 = 4G
4 = 5G
```

## 2. User enters the number of devices to add

* Input is validated using `basicIO`
* Exceptions thrown for invalid menu choice

## 3. System computes channel & core capacity
Bandwidth-based capacity:

```
channels = totalBandwidth / channelBandwidth
perChannel = usersPerChannel × antennas
maxUsersBandwidth = channels × perChannel
``
Core-based capacity:

```
coreMaxUsers = (coreMessageCapacity * 100) / (messagesPerUser × (100 + overhead))
```

The *actual* number of users added is:

```
finalUsers = min(requestedUsers, bandwidthCapacity, coreCapacity)
```

## 4. Channel Allocation

Users are placed into channels:

```
channel 1 → users 1 … N
channel 2 → next N users
...
```

Channel listing shows only active channels.

## 5. Thread vs Non-thread Time Simulation

* Uses templates (`div_up<long long>`) for time calculations
* Non-threaded time simulates a single core
* Threaded time distributes messages across cores
* Prints speedup and per-thread distribution

## 6. Comparison Across All Standards

* Channel count
* Bandwidth capacity
* Core-limited capacity
* Technology speed

---

Building & Running

Compile using the provided Makefile:

### Debug build:

```
make debug
```

Generates:

```
cellsim_debug
```

### Release build:

```
make release
```

Generates:

```
cellsim
```

Run the simulator:

```
./cellsim
```

 Conclusion

This simulator demonstrates a complete object-oriented model of cellular technologies while respecting strict constraints such as **no I/O stdlib**, **use of templates**, **exception handling**, and **threading comparison**. The architecture is extensible, modular, and accurately reflects resource constraints in communication systems.
