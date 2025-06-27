# Recursive Flow Classification (RFC) Algorithm Implementation

This repository provides an implementation of the **Recursive Flow Classification (RFC)** algorithm, as introduced in the seminal paper [*Packet Classification on Multiple Fields*](https://dl.acm.org/doi/10.1145/316194.316229) (SIGCOMM '99) by Srinivasan et al. RFC is a high-performance algorithm designed for efficient packet classification based on multiple header fields, particularly the standard 5-tuple used in network flows.

## 📌 Overview

Packet classification is a fundamental component in many networking applications such as firewalls, QoS, traffic shaping, and intrusion detection systems. The RFC algorithm offers a practical approach for fast and scalable multi-field packet classification by transforming high-dimensional matching problems into a series of smaller lookup problems.

This project aims to:

- Reproduce the original RFC algorithm based on the SIGCOMM'99 paper.
- Provide a modular, well-documented implementation suitable for research and educational purposes.
- Offer example datasets and testing scripts to validate correctness and performance.

## 📌 pipeline

RFC leverages field decomposition to construct a pipelined classifier. By splitting rule fields into fixed-size chunks and organizing them into staged lookup tables, it simplifies classification logic and supports hardware-friendly deployment.
Pipeline structure is illustrated below:![rfc](https://github.com/user-attachments/assets/5bf264ed-1dc1-4c4c-a768-52eedca0744f)


## 🔧 Features

- Support for standard 5-tuple classification: source IP, destination IP, source port, destination port, and protocol.
- Preprocessing steps for rule expansion and field slicing.
- Recursive reduction of rule sets using equivalence class mapping and table generation.
- Final decision table construction and runtime classification engine.
- Sample rule sets and synthetic traffic traces for evaluation.

## 🚀 Getting Started

```bash
# clone the project
git clone https://github.com/kugaichen/RFC.git 

# build
mkdir build
cd build

#make
cmake ..
make

#run
cd ..
./main <filter_filepath> <packet_filepath> [output_log_filepath]
#example ./main ./dataset/text_filter.txt ./dataset/text_packet.txt ./rfc_info_output.log
