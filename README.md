# IEX2H5: IEX TOPS Dataset to HDF5 Converter

A high-performance C++ utility for converting [IEX Transport Protocol (IEX-TP)][101] packet captures into structured HDF5 datasets, suitable for financial analytics, scientific computation, and time-series processing.

---

## 📦 Installation

1. **Install Intel oneAPI** (for Intel compilers and MKL)  
   Download from: [Intel oneAPI Base Toolkit](https://www.intel.com/content/www/us/en/developer/tools/oneapi/base-toolkit-download.html)

2. **Install required libraries**

```bash
sudo apt install libgtest-dev libhdf5-dev libboost-program-options-dev \
    libboost-system-dev libgoogle-glog-dev libgoogle-perftools-dev libcpprest-dev libpcap-dev pigz
# intall Howard Hinnant's date library
git clone https://github.com/HowardHinnant/date.git && cd date
cmake -DBUILD_TZ_LIB=ON . && make && sudo make install
```
# Example Usage: Convert IEX TOPS Dataset
```
steven@jupyter:~/projects/iex2h5$ iex2h5 --help
IEX2H5 converts IEX TOPS Datasets to HDF5 Format

iex2h5 is a specialized tool for importing IEX TOPS datasets into the HDF5 data format,
enabling efficient  storage and analysis of large  financial datasets. HDF5 is a widely
used file format for handling large, complex, and hierarchical data, supported by major
programming languages including Julia, Python, MATLAB, C, C++, and Node.js.

This application allows users to convert captured packet data streams (e.g., DEEP/TOPS)
into structured HDF5 datasets for advanced analytics and seamless integration into
scientific, engineering, and financial workflows.
Allowed options:
  --time-interval arg (=10)                    temporal interval in seconds, irts stream is converted into
  --start arg (=14:30:00)                      lower bound on processing data stream 
  --stop arg (=21:00:00)                       upper bound on processing stream
                                               
  -i [ --input ] arg                           packet capture file or when left empty: stdin
  -o [ --output ] arg (=./iex.h5)              output hdf5 file
  -r [ --rts ] arg (=/time.txt)                hdf5-group/directory for regular time interval datasets
  --asset-path arg (=/instruments.txt)         path to HDF5 index dataset for listed [symbols|assets|financial] 
                                               instruments
  --trading-days-path arg (=/trading_days.txt) path to HDF5 dataset containing the list of trading days
  -g [ --gzip ] arg (=0)                       0-9 0 for no compression, 9 for highest
  -c [ --chunk ] arg (=1)                      number of days in blocks/hdf5-chunks, 0 no-chunks 
                                               
                                               
  --command arg                                irts    - saves captured events as irts stream
                                               rts     - converts irts to rts
                                               assets  - retrieves symbols from irts/stream
                                               index   - creates trading days
                                               
                                               
  --glog-dir arg (=./)                         glog output directory
  --glog-stderr arg (=1)                       glog output to stderr if true
  --glog-minloglevel arg (=0)                  glog log level:  INFO=0 WARNING=1 ERROR=2 FATAL=3
                                               
  -h [ --help ]                                produce help message

example:
   iex2h5
Copyright © <2017-2025> Varga Consulting, Toronto, ON, info@vargaconsulting.ca
```
