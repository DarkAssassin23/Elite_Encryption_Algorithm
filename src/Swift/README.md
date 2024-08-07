# Elite Encryption Algorithm (Swift Implementation)

## Table of Contents 
 * [About](#about)
 * [Setup](#Setup)
 * [Building](#building)
 * [Usage](#usage)
 
## About
This is an implementation of the Elite Encryption Algorithm (EEA), in Swift, 
that allows you to generate keys, encrypt, and decrypt data. If you want a
breakdown of how the algorithm works and how keys are generated, check out my 
[blog post](https://chiefwithcolorfulshoes.com/blog/Elite_Encryption_Algorithm/) 

> [!WARNING]
> This implementation encrypts your keys via password. There are some
> checks to help make sure you enter your password correctly, but it is not 
> full-proof.

## Setup
You first need to install Swift. For install instructions refer to the
[offical Swift documentation](https://www.swift.org/install/).

If you are on Windows, the easiest way I found to build the project is to make
sure you are using the `x64 Visual Studio Developer Command Prompt` and
enabling `Developer Mode` in the `Settings` under `System`.

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
The resulting executable will be placed in the root of this directory.

> [!NOTE]
> If you are on Windows, use `nmake` instead of `make`.

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

Like the [C](../C/) and [Java](../Java/) implementations, you are able to have
as many key files as you like and can select which one to use at the time of 
encryption and decryption.

## Encryption and Decryption
Both encryption and decryption modes allow you to encrypt or decrypt
single files, directories, or strings of text. The directories are encrypted
recursively, so any and all files inside that directory will be encrypted.
As for decryption, only `.eea` files will be decrypted, but, like with
encryption, decryption is done recursively when performed on directories.

### Ghost Mode
All encryption and decryption methods have a mode called Ghost Mode.
In Ghost Mode, you have the ability to either manually enter 
(in the case of decryption) or create one-time-use keys. 
Ghost Mode does not utilize any of your saved keys files, nor does it 
generate a new one. The reason it is called "Ghost Mode" is so your 
encryption/decryption are like a ghost and your keys only ever exist in 
memory. Meaning, the keys can never be recovered, even by data recovery, 
since they were never saved to your hard drive/solid-state drive. 
The only way you would be able to decrypt data encrypted with Ghost Mode
is if you manually copied the keys yourself and then manually re-entered
them using Ghost Mode to decrypt.
