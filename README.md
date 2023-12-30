# Firewall Rule Management System

## Overview
This project is a part of the "Operating Systems and Systems Programming" course at The University of Birmingham. It involves writing programs to parse and manage firewall rules, including checking whether a packet with a given destination and port should be accepted or rejected based on the defined rules.

## Description
The assignment comprises two main programs:
1. **readFirewall**: Reads firewall rules from a file, outputs sorted rules, and reports ill-formed rules.
2. **checkFirewall**: Extends readFirewall to check if a packet with a specific IP address and port should be accepted based on the rules.

## Features
- Parsing and validation of firewall rules.
- Sorting rules based on port numbers and IP addresses.
- Checking the acceptability of packets against firewall rules.

## Installation
### Prerequisites
- GCC Compiler
- Linux environment

### Compiling
To compile the programs, use the following commands:
```bash
gcc -o readFirewall readFirewall.c
gcc -o checkFirewall checkFirewall.c
```

## Usage
### readFirewall
```bash
./readFirewall <filename>
```
### checkFirewall
```bash
./checkFirewall <filename> <IPaddress> <port>
```

## Programs

### readFirewall
This program reads firewall rules from a file and sorts them based on port numbers and IP addresses. It handles and reports any ill-formed rules encountered.

#### Code Structure
- **File Reading**: Reads rules from a file.
- **Rule Parsing**: Parses and validates each rule.
- **Error Handling**: Identifies and reports ill-formed rules.
- **Sorting**: Sorts the rules using the `qsort` function.

### checkFirewall
This program extends `readFirewall` to check if a packet with a specific IP address and port is accepted by the firewall rules.

#### Code Structure
- **Rule Checking**: Determines if a packet is accepted based on rules.
- **IP Address and Port Parsing**: Parses and validates the input IP address and port.
- **Acceptance Logic**: Implements the logic to decide if a packet should be accepted or rejected.

