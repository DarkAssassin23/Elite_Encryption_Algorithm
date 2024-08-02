# Elite Encryption Algorithm (Java Desktop App)

## Table of Contents
 * [About](#about)
 * [Building](#building)
 * [Usage](#usage)

## About
This is an implementation of the Elite Encryption Algorithm (EEA), in Java, 
that allows you to generate keys, encrypt, and decrypt data. If you want a
breakdown of how the algorithm works and how keys are generated, check out my 
[blog post](https://chiefwithcolorfulshoes.com/blog/Elite_Encryption_Algorithm/) 
> [!WARNING]
> As of Aug. 1<sup>st</sup>, 2024, This implementation encrypts your keys via
> password. There are some checks to help make sure you enter your password
> correctly, but it is not full-proof.

## Building
> [!NOTE]
> This assumes you already have Java installed and configured to be able to
> compile and run Java apps from the command line. If you don't, you will get
> errors when running the `makefile`.

Use the included `makefile` in order to build the `.jar` file to run the
application.

To build the applications `.jar` file, navigate to the folder of the
`makefile` and run the following command:
```bash
make jar
```
This will compile and build the application. To run the application,
double-click on the `EEA_App.jar` file. Alternatively, you can run it from
the command line with:
```bash
java -jar EEA_App.jar
```

## Usage
As mentioned, this application gives you the ability to generate keys,
encrypt, and decrypt data. This data can be text, individual files, or
directories.

When encrypting files and directories, you can choose to save the encrypted
file(s) as a new file or overwrite the existing file. When decrypting,you
have the same ability. You have the option of overwriting the encrypted
file(s), effectively deleting them, or keeping the encrypted files and adding
the additional decrypted files.

### Key Generation
This application gives you the ability to generate your own keys, as well as
delete them, if you choose to do so. When you create a new set of keys, the
app will prompt you to choose how many keys you would like to generate and
how long you want your keys to be. It will then prompt you for a password in
order to encrypt your keys.

> [!NOTE]
> As of Aug. 1<sup>st</sup>, 2024, This implementation now allows you to have
> as many key files as you like and can select which one to use at the time
> of encryption and decryption.

### Ghost Mode
In Ghost Mode you have the ability to either manually enter (in the case of
decryption), or create one-time use keys. Ghost Mode does not utilize your
saved keys file, nor does it generate a new one. The reason it is called
"Ghost Mode" is so your encryption/decryption is like a ghost and your keys
only ever exist in memory. Meaning, the keys can never be recovered, even by
data recovery, since they were never saved to your 
hard drive/solid-state drive. The only way you would be able to decrypt data
encrypted with Ghost Mode is if you manually copied the keys yourself and then
manually re-entered them using Ghost Mode to decrypt.
