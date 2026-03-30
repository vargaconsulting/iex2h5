
[![CI](https://github.com/vargalabs/iex2h5/actions/workflows/ci.yml/badge.svg)](https://github.com/vargalabs/iex2h5/actions/workflows/ci.yml)
[![codecov](https://codecov.io/gh/vargalabs/iex2h5/branch/main/graph/badge.svg)](https://codecov.io/gh/vargalabs/iex2h5)
[![MIT License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)
[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.15677290.svg)](https://doi.org/10.5281/zenodo.15677290)
[![GitHub release](https://img.shields.io/github/v/release/vargalabs/iex2h5.svg)](https://github.com/vargalabs/iex2h5/releases)
[![Documentation](https://img.shields.io/badge/docs-stable-blue)](https://vargalabs.github.io/iex2h5)

# IEX2H5: IEX TOPS Dataset to HDF5 Converter
A high-performance C++ utility for converting [IEX Transport Protocol (IEX-TP)][100] packet captures into structured HDF5 datasets, suitable for financial analytics, scientific computation, and time-series processing.

## Build Matrix

| OS / Compiler | GCC 13      | GCC 14      | GCC 15      | Clang 17      | Clang 18      | Clang 19      |Clang 20       |
|---------------|-------------|-------------|-------------|---------------|---------------|---------------|---------------|
| Ubuntu 22.04  |![gcc13][200]|![gcc14][201]|![gcc15][202]|![clang17][250]|![clang18][251]|![clang19][252]|![clang20][253]|
| Ubuntu 24.04  |![gcc13][300]|![gcc14][301]|![gcc15][302]|![clang17][350]|![clang18][351]|![clang19][352]|![clang20][353]|

## 📦 Installation
```bash
sudo apt install libhdf5-dev pigz
mkdir build && cd build && cmake ../
make -j 12 && sudo make install
```


### Notice:
“[Data provided][100] for free by IEX. By accessing or using IEX Historical Data, you agree to the [IEX Historical Data Terms of Use][101].”

[100]: https://iextrading.com/trading/market-data/
[101]: https://www.iexexchange.io/legal/hist-data-terms
[200]: https://vargalabs.github.io/iex2h5/badges-staging/ubuntu-22.04-gcc-13.svg
[201]: https://vargalabs.github.io/iex2h5/badges-staging/ubuntu-22.04-gcc-14.svg
[202]: https://vargalabs.github.io/iex2h5/badges-staging/ubuntu-22.04-gcc-15.svg
[300]: https://vargalabs.github.io/iex2h5/badges-staging/ubuntu-24.04-gcc-13.svg
[301]: https://vargalabs.github.io/iex2h5/badges-staging/ubuntu-24.04-gcc-14.svg
[302]: https://vargalabs.github.io/iex2h5/badges-staging/ubuntu-24.04-gcc-15.svg
[250]: https://vargalabs.github.io/iex2h5/badges-staging/ubuntu-22.04-clang-17.svg
[251]: https://vargalabs.github.io/iex2h5/badges-staging/ubuntu-22.04-clang-18.svg
[252]: https://vargalabs.github.io/iex2h5/badges-staging/ubuntu-22.04-clang-19.svg
[253]: https://vargalabs.github.io/iex2h5/badges-staging/ubuntu-22.04-clang-20.svg
[350]: https://vargalabs.github.io/iex2h5/badges-staging/ubuntu-24.04-clang-17.svg
[351]: https://vargalabs.github.io/iex2h5/badges-staging/ubuntu-24.04-clang-18.svg
[352]: https://vargalabs.github.io/iex2h5/badges-staging/ubuntu-24.04-clang-19.svg
[353]: https://vargalabs.github.io/iex2h5/badges-staging/ubuntu-24.04-clang-20.svg