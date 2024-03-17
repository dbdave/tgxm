# TGXm

[![Github All Releases](https://img.shields.io/github/downloads/tgxm/tgxm/total.svg)](https://github.com/tgxm/tgxm/releases)
[![GitHub release](https://img.shields.io/github/release/tgxm/tgxm/all.svg)](https://github.com/tgxm/tgxm/releases)
[![GitHub Release Date](https://img.shields.io/github/release-date/tgxm/tgxm.svg)](https://github.com/tgxm/tgxm/releases)
[![GitHub license](https://img.shields.io/github/license/tgxm/tgxm.svg)](https://github.com/tgxm/tgxm/blob/master/LICENSE)
[![GitHub stars](https://img.shields.io/github/stars/tgxm/tgxm.svg)](https://github.com/tgxm/tgxm/stargazers)
[![GitHub forks](https://img.shields.io/github/forks/tgxm/tgxm.svg)](https://github.com/tgxm/tgxm/network)

TGXm is a high performance, open source, cross platform RandomX, KawPow, CryptoNight and [GhostRider](https://github.com/tgxm/tgxm/tree/master/src/crypto/ghostrider#readme) unified CPU/GPU miner and [RandomX benchmark](https://tgxm.com/benchmark). Official binaries are available for Windows, Linux, macOS and FreeBSD.

## Mining backends
- **CPU** (x86/x64/ARMv7/ARMv8)
- **OpenCL** for AMD GPUs.
- **CUDA** for NVIDIA GPUs via external [CUDA plugin](https://github.com/tgxm/tgxm-cuda).

## Download
* **[Binary releases](https://github.com/tgxm/tgxm/releases)**
* **[Build from source](https://tgxm.com/docs/miner/build)**

## Usage
The preferred way to configure the miner is the [JSON config file](https://tgxm.com/docs/miner/config) as it is more flexible and human friendly. The [command line interface](https://tgxm.com/docs/miner/command-line-options) does not cover all features, such as mining profiles for different algorithms. Important options can be changed during runtime without miner restart by editing the config file or executing [API](https://tgxm.com/docs/miner/api) calls.

* **[Wizard](https://tgxm.com/wizard)** helps you create initial configuration for the miner.
* **[Workers](http://workers.tgxm.info)** helps manage your miners via HTTP API.

## Donations
* Default donation 1% (1 minute in 100 minutes) can be increased via option `donate-level` or disabled in source code.
* XMR: `48edfHu7V9Z84YzzMa6fUueoELZ9ZRXq9VetWzYGzKt52XU5xvqgzYnDK9URnRoJMk1j8nLwEVsaSWJ4fhdUyZijBGUicoD`

## Developers
* **[tgxm](https://github.com/tgxm)**
* **[sech1](https://github.com/SChernykh)**

## Contacts
* support@tgxm.com
* [reddit](https://www.reddit.com/user/TGXm/)
* [twitter](https://twitter.com/tgxm_dev)
