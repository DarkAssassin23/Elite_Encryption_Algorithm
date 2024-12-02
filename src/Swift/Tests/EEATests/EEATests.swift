import Crypto
import EEAUtils
import Testing

/// Struct To handle testing various components of EEA
struct EEASwiftTests {
    /// Test case to make sure key generation works
    @Test func keyGenTest() {
        let keygen = Keygen()
        let crypto = EEA()
        var keys: [String]
        var passed: Bool = true
        do {
            for x in (1...10) {
                let size: Int = Int.random(in: 1...10) * 256
                keys = try keygen.genKeys(size: size, num: x)
                #expect(
                    keys.count == x,
                    "Key Generation Test Failed: Number of keys invalid"
                )
                for key in keys {
                    let length = (size / 4)
                    #expect(
                        key.count == length,
                        "Key Generation Test Failed: Key length invalid"
                    )
                }
                try crypto.keyCheck(keys)
            }
        } catch (_) {
            passed = false
        }
        #expect(passed, "Key generation test failed")
    }

    /// Test case to make sure encrypting and decrypting text works
    @Test func stringEncryptTest() {
        let keygen = Keygen()
        let crypto = EEA()
        var keys: [String]!
        var passed: Bool = true
        do {
            keys = try keygen.genKeys()
        } catch (_) {
            passed = false
        }
        #expect(passed, "String Encrypt Test Error: Key generation")

        let ogString: String = "This is a test string.\nIt will be encrypted."
        guard let crypt = crypto.encryptText(text: ogString, keys: keys) else {
            passed = false
            #expect(passed, "String Encrypt Test Error: Encrypting text")
            return
        }
        guard let decrypt = crypto.decryptText(text: crypt, keys: keys) else {
            passed = false
            #expect(passed, "String Encrypt Test Error: Decrypting text")
            return
        }
        passed = decrypt == ogString
        #expect(passed, "String Encrypt Test Failed")
    }

    /// Test case to make sure encrypting and decrypting files works
    @Test func fileEncryptTest() {
        let keygen = Keygen()
        let crypto = EEA()
        let io = FileIO()
        var keys: [String]!
        var passed: Bool = true
        do {
            keys = try keygen.genKeys()
        } catch (_) {
            passed = false
        }
        #expect(passed, "File Encrypt Test Error: Key generation")

        let filename = "output.txt"
        let ogString: String = "This is a test string.\nIt will be encrypted."
        let data: [UInt8] = Array(ogString.utf8)
        _ = try? io.writeFile(data, filename: filename)
        _ = try? crypto.encryptFile(inFile: filename, keys: keys)
        _ = try? crypto.decryptFile(
            inFile: String("\(filename).eea"),
            keys: keys
        )
        let tmp = try? io.readFile(filename)

        // Cleanup
        let deleted = io.deleteFile(filename: filename)
        #expect(deleted, "File Encrypt Test Error: Deleting \(filename)")

        passed = passed && (tmp == data)
        #expect(passed, "File Encrypt Test Failed")
    }
}
