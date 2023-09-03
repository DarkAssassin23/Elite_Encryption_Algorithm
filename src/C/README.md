# Elite Encryption Algorithm (C implementation)

## About
The Elite Encryption Algorithm (EEA) implemented in C
> **Warning**:
> This implementation encrypts your keys via password. There are some
> checks to help make sure you enter your password correctly, but it is not 
> full-proof.

## Dependencies
This implementation of EEA depends on having `openssl`. It can be installed
by running one of the following commands below depending on your OS:

### macOS
```bash
brew install openssl
```
### Linux
#### Debian-based
```bash
apt install libssl-dev
```
#### Red Hat-based
```bash
dnf install openssl
```
#### Arch-based
```bash
pacman -S install openssl openssl-devel
```
### Windows (using MSYS2)
```bash
pacman -S install openssl openssl-devel
```
> **Note**:
> This implementation has been built and tested against 
> [MSYS2](https://www.msys2.org) **NOT** Visual Studio. It is advised
> that you either use MSYS2 or Windows Subsystem for Linux (WSL), and 
> follow the Linux instructions, instead. 