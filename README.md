# qtrng
Qt Random number generator wrapper around "secure" os RNGS

## Features
- provides a simple, unseeded RNG
	- Uses /dev/(u)random on unix
	- Uses CryptGenRandom on Windows
- 2 Security levels (Unix only, random vs urandom)
- Report entropy level for high security on linux

## Installation
The package is providet as qpm package, [`de.skycoder42.qrng`](https://www.qpm.io/packages/de.skycoder42.qrng/index.html). To install:

1. Install qpm (See [GitHub - Installing](https://github.com/Cutehacks/qpm/blob/master/README.md#installing), for **windows** see below)
2. In your projects root directory, run `qpm install de.skycoder42.qrng`
3. Include qpm to your project by adding `include(vendor/vendor.pri)` to your `.pro` file

Check their [GitHub - Usage for App Developers](https://github.com/Cutehacks/qpm/blob/master/README.md#usage-for-app-developers) to learn more about qpm.

**Important for Windows users:** QPM Version *0.10.0* (the one you can download on the website) is currently broken on windows! It's already fixed in master, but not released yet. Until a newer versions gets released, you can download the latest dev build from here:
- https://storage.googleapis.com/www.qpm.io/download/latest/windows_amd64/qpm.exe
- https://storage.googleapis.com/www.qpm.io/download/latest/windows_386/qpm.exe

## Notes
Please note that QRng does not support reseeding, adding entropy or algorithm selection. It simply uses the default OS provided secure RNGs. Thus, the quality of the generated data can not be guaranteed. Read the specific documentations of the OS if you want to know more about the security of these. If you are implementing high security algorithms, you should use this class to generate seeds only, and use a proper PRNG implementation on top of it.
