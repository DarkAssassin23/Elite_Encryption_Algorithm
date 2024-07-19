import Foundation

extension String {
    func fromBase64() -> String? {
        guard let data = Data(base64Encoded: self) else {
            return nil
        }

        return String(data: data, encoding: .utf8)
    }

    func toBase64() -> String {
        return Data(self.utf8).base64EncodedString()
    }
}

/*============================================*/
/*               Error Types                  */
/*============================================*/
enum KeyError: Error {
    case invalidKey(String)
    case invalidLength(String)
    case lengthMismatch(String)
    case noKeys(String)
}

enum Base64Error: Error {
    case encodeFailed(String)
    case decodeFailed(String)
}

enum EEAError: Error {
    case notEEA(String)
    case badKeyLen(String)
}

/// A class to handle encrypting and decrypting data using the
/// Elite Encryption Algorithm
public struct EEA {
    private let io: FileIO
    private let minSize: Int

    public init() {
        self.io = FileIO()
        self.minSize = 64
    }

    /// Ensure all the keys are of the same length and valid
    ///
    /// If a key is not valid, an exception will be thrown
    /// - Parameter keys: The keys to check the validity of
    public func keyCheck(_ keys: [String]) throws {
        guard let size = keys.first?.count else {
            throw KeyError.noKeys(
                "Error: No keys were provided"
            )
        }
        if size % minSize != 0 {
            throw KeyError.invalidLength(
                "The key length, \(size), is not a multiple of \(minSize)."
            )
        }

        for key in keys {
            if key.count != size {
                throw KeyError.lengthMismatch(
                    "Error: Key had length \(key.count), expected: \(size)."
                )
            }
            if key.range(of: "^[a-fA-F0-9]*$", options: .regularExpression)
                == nil
            {
                throw KeyError.invalidKey(
                    "\(key) is not a valid key, it should be a hex value."
                )
            }
        }
    }

    /*============================================*/
    /*                Encryption                  */
    /*============================================*/
    /// Given data and a key, encrypt the data and return the result
    /// - Parameters:
    ///   - data: Array containing the data to be encrypted
    ///   - key: The key to be used for encryption
    /// - Returns: The encrypted cipher text
    private func encryptOnce(data: [UInt8], key: String) -> [UInt8] {
        var keyData = Array(key.utf8)
        var prevBlock: [UInt8] = [UInt8](repeating: 0, count: key.count)
        var plainText: [UInt8] = data

        // Add padding to the data if needed
        while plainText.count % key.count != 0 {
            plainText.append(0)
        }
        var cipher: [UInt8] = [UInt8](repeating: 0, count: plainText.count)

        // Perform the encryption
        var count: Int = 0
        var i: Int = 0
        for byte in plainText {
            let b = byte ^ keyData[count]
            cipher[i] = b
            prevBlock[count] = b
            count += 1
            i += 1
            if count == keyData.count {
                count = 0
                keyData = prevBlock
            }
        }
        return cipher
    }

    /// Given an encrypted cipher text, encode it in base64
    /// - Parameter data: Cipher text to be encoded
    /// - Returns: The cipher texted encoded as a base64 string
    public func encode(data: [UInt8]) throws -> String {
        guard let coded = String(bytes: data, encoding: .utf8)?.toBase64()
        else {
            throw Base64Error.encodeFailed(
                "Error: Encoding data to base64 failed."
            )
        }
        return coded
    }

    /// Given an string of text and a list of keys, encrypt the data
    /// - Parameters:
    ///   - data: Array containing the data to be encrypted
    ///   - keys: The keys to be used for encryption
    /// - Returns: The encrypted cipher text
    public func encryptText(text: String, keys: [String]) -> String? {
        var cipherText: [UInt8] = Array(text.utf8)
        do {
            try keyCheck(keys)
            for key in keys {
                cipherText = encryptOnce(data: cipherText, key: key)
            }

            let txt = try encode(data: cipherText)
            return txt
        } catch KeyError.invalidLength(let e), KeyError.invalidKey(let e),
            KeyError.lengthMismatch(let e)
        {
            print(e)
        } catch Base64Error.encodeFailed(let e) {
            print(e)
        } catch (let e) {
            print("Caught unknown error: \(e)")
        }
        return nil
    }

    /// Given an array of data and a list of keys, encrypt the data
    /// - Parameters:
    ///   - data: Array containing the data to be encrypted
    ///   - keys: The keys to be used for encryption
    /// - Returns: The encrypted cipher text
    public func encrypt(data: [UInt8], keys: [String]) throws -> [UInt8] {
        do {
            try keyCheck(keys)
        } catch (let e) {
            throw e
        }

        var cipherText: [UInt8] = data
        for key in keys {
            cipherText = encryptOnce(data: cipherText, key: key)
        }

        return cipherText
    }

    /// Encrypt the given input file with the specified keys
    /// - Parameters:
    ///   - inFile: Name of the file to be encrypted
    ///   - keys: List of keys to be used for encryption
    ///   - outFile: File the cipher text should be written too
    ///              (default: the input file with an '.eea' extention)
    /// > Note: If outFile is `nil` the file will be overwritten.
    ///   I.e. The `inFile` file will be deleted
    ///
    /// - Returns: If the file was successfully encrypted
    public func encryptFile(
        inFile: String,
        keys: [String],
        outFile: String? = nil
    ) throws -> Bool {
        do {
            try keyCheck(keys)
            let data = try io.readFile(inFile)
            let cipherData = try encrypt(data: data, keys: keys)
            let cipherText = try encode(data: cipherData)

            guard let outFile = outFile else {
                let outFile = inFile + ".eea"
                let ret = try io.writeFile(
                    Array(cipherText.utf8),
                    filename: outFile
                )
                _ = io.deleteFile(filename: inFile)
                return ret
            }
            if !outFile.hasSuffix(".eea") {
                throw EEAError.notEEA(
                    "The file you are writing to should end in .eea."
                )
            }
            let ret = try io.writeFile(
                Array(cipherText.utf8),
                filename: outFile
            )
            return ret
        } catch (let e) {
            throw e
        }
    }

    /*============================================*/
    /*                Decryption                  */
    /*============================================*/
    /// Gets the previous block
    /// - Parameters:
    ///   - data: The data to decrypt
    ///   - start: The start index of the next block
    ///   - key: The current key being used for decryption
    /// - Returns: The previous block
    private func getPrevBlock(_ data: [UInt8], _ start: Int, _ key: String)
        -> [UInt8]
    {
        let keyDataLen = Array(key.utf8).count
        let end = start + keyDataLen
        return (start >= 0) ? Array(data[start..<end]) : Array(key.utf8)
    }

    /// Remove excess padding at the end of the plain text added during
    /// encryption
    /// - Parameter data: The plain text with the padding to remove
    /// - Returns: The plain text with its padding removed
    private func removePadding(_ data: [UInt8]) -> [UInt8] {
        var plainText = data
        while let byte = plainText.last {
            if byte == 0 {
                plainText.removeLast()
            } else {
                break
            }
        }
        return plainText
    }

    /// Given data and a key, decrypt the data and return the result
    /// - Parameters:
    ///   - data: Array containing the data to be decrypted
    ///   - key: The key to be used for encryption
    /// - Returns: The cipher text decrypted into plain text
    private func decryptOnce(data: [UInt8], key: String) -> [UInt8] {
        let keyDataLen = Array(key.utf8).count
        let initialStart = data.count - (keyDataLen * 2)
        var plain: [UInt8] = [UInt8](repeating: 0, count: data.count)
        var keyData: [UInt8] = getPrevBlock(data, initialStart, key)

        // Perform the decryption
        var count: Int = 0
        var i: Int = data.count - 1
        var keyIndex: Int = keyDataLen - 1
        for byte in data.reversed() {
            // Check if we need to reset the key for the next block
            if count == keyDataLen {
                let prevBlockStart: Int = i - ((keyDataLen * 2) - 1)
                keyData = getPrevBlock(data, prevBlockStart, key)
                keyIndex = keyDataLen - 1
                count = 0
            }
            plain[i] = keyData[keyIndex] ^ byte
            i -= 1
            keyIndex -= 1
            count += 1
        }
        return plain
    }

    /// Given an encrypted cipher text, decode it from base64
    /// - Parameter data: Cipher text to be decoded
    /// - Returns: The cipher texted decoded from base64
    public func decode(data: String) throws -> [UInt8] {
        guard let decoded = data.fromBase64() else {
            throw Base64Error.decodeFailed(
                "Decoding the data from base64 failed."
            )
        }
        return Array(decoded.utf8)
    }

    /// Given an string of text and a list of keys, decrypt the data
    /// - Parameters:
    ///   - text: Array containing the data to be decrypted
    ///   - keys: The keys to be used for decryption
    /// - Returns: The decrypted plain text
    public func decryptText(text: String, keys: [String]) -> String? {
        do {
            try keyCheck(keys)

            let decoded = try decode(data: text)
            var plainText: [UInt8] = decoded
            if plainText.count % keys[0].count != 0 {
                print("Error: Size of the data doesn't match the key length.")
                return nil
            }

            for key in keys.reversed() {
                plainText = decryptOnce(data: plainText, key: key)
            }

            plainText = removePadding(plainText)
            return String(bytes: plainText, encoding: .utf8)
        } catch KeyError.invalidLength(let e), KeyError.invalidKey(let e),
            KeyError.lengthMismatch(let e)
        {
            print(e)
        } catch Base64Error.encodeFailed(let e), Base64Error.decodeFailed(
            let e
        )
        {
            print(e)
        } catch (let e) {
            print("Caught unknown error: \(e)")
        }
        return nil
    }

    /// Given an array of data and a list of keys, decrypt the data
    /// - Parameters:
    ///   - data: Array containing the data to be decrypted
    ///   - keys: The keys to be used for decryption
    /// - Returns: The data decrypted
    public func decrypt(data: [UInt8], keys: [String]) -> [UInt8] {
        var cipherText: [UInt8]
        // Using & since it is not in base64
        let dataStr = String(bytes: data, encoding: .utf8) ?? "&"

        // Try to decode the data from base64
        if let tmp = try? decode(data: dataStr) {
            cipherText = tmp
        } else {
            cipherText = data
        }
        for key in keys.reversed() {
            cipherText = decryptOnce(data: cipherText, key: key)
        }
        return removePadding(cipherText)
    }

    /// Decrypt the given input file with the specified keys
    /// - Parameters:
    ///   - inFile: Name of the file to be decrypted
    ///   - keys: List of keys to be used for decryption
    ///   - outFile: File the plain text should be written too
    ///              (default: the input file minus the `.eea` extention)
    /// > Note: If outFile is `nil` the file will be overwritten.
    ///   I.e. The `.eea` file will be deleted
    ///
    /// - Returns: If the file was successfully decrypted
    public func decryptFile(
        inFile: String,
        keys: [String],
        outFile: String? = nil
    ) throws -> Bool {
        if !inFile.hasSuffix(".eea") {
            throw EEAError.notEEA(
                "The file should end in a .eea extension"
            )
        }
        do {
            try keyCheck(keys)
            let data = try io.readFile(inFile)
            let plainText = decrypt(data: data, keys: keys)
            guard let outFile = outFile else {
                // Remove .eea extension
                let index = inFile.lastIndex(of: ".")!
                let outFile = String(inFile[inFile.startIndex..<index])
                let ret = try io.writeFile(plainText, filename: outFile)
                _ = io.deleteFile(filename: inFile)
                return ret
            }

            if outFile.hasSuffix(".eea") {
                throw EEAError.notEEA(
                    "The file you are writing to should not end in .eea."
                )
            }
            let ret = try io.writeFile(plainText, filename: outFile)
            return ret
        } catch (let e) {
            throw e
        }
    }
}
