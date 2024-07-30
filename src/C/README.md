# Elite Encryption Algorithm (C Implementation)

## Table of Contents 
 * [About](#about)
 * [Dependencies](#dependencies)
   * [macOS](#macos)
   * [Linux](#linux)
   * [Windows](#windows-using-msys2)
 * [Building](#building)
 * [Usage](#usage)
 
## About
This is an implementation of the Elite Encryption Algorithm (EEA), in C, 
that allows you to generate keys, encrypt, and decrypt data. If you want a
breakdown of how the algorithm works and how keys are generated, check out my 
[blog post](https://chiefwithcolorfulshoes.com/blog/Elite_Encryption_Algorithm/) 

> [!WARNING]
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
> [!NOTE]
> This implementation has been built and tested against 
> [MSYS2](https://www.msys2.org) **NOT** Visual Studio. It is advised
> that you either use MSYS2 or Windows Subsystem for Linux (WSL), and 
> follow the Linux instructions, instead. 

## Building
You can build either `release` or `debug` variants. Though, it is 
recommend to build the release version for greater optimization and
smaller file size.
```bash
# Build release
make release
``` 
```bash
# Build debug
make debug
``` 

## Usage
This implementation utilizes a command-line interface (CLI) to allow
you to manage your keys as well as encrypt and decrypt data.

### Configuration
When you run EEA, it will create a config file, `eea.conf`, if one doesn't
already exist. This is where you can specify where EEA should look for your
`.keys` files, or the files that store your keys used for encryption and
decryption

### Key Generation
The CLI gives you the ability to generate your own keys as well as delete
them if you choose to do so. When you create a new set of keys, the app will
prompt you to choose how many keys you would like to generate and how long 
you want your keys to be. After which, it will prompt you to enter a password
so your keys can be encrypted.

Unlike the [Java implementation](../Java/), you are able to have as many key
files as you like and can select which one to use at the time of 
encryption and decryption.

## Encryption and Decryption
Both encryption and decryption modes allow you to either encrypt or decrypt
single files or directories. The directories are encrypted recursively, so
any and all files inside that directory will be encrypted. As for
decryption, only `.eea` files will be decrypted, but, like with encryption,
decryption is done recursively when performed on directories.

### Ghost Mode
Both encryption and decryption on single files and directories has a mode 
called Ghost Mode. In Ghost Mode, you have the ability to either manually 
enter (in the case of decryption) or create one-time-use keys. 
Ghost Mode does not utilize any of your saved keys files, nor does it 
generate a new one. The reason it is called "Ghost Mode" is so your 
encryption/decryption are like a ghost and your keys only ever exist in 
memory. Meaning, the keys can never be recovered, even by data recovery, 
since they were never saved to your hard drive/solid-state drive. 
The only way you would be able to decrypt data encrypted with Ghost Mode
is if you manually copied the keys yourself and then manually re-entered
them using Ghost Mode to decrypt.
