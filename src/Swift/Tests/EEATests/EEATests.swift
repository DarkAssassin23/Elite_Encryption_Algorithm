import Crypto
import EEAUtils
import XCTest

enum TestError: Error {
    case nilValue(String)
}

/// Class To handle testing various components of EEA
class EEATests: XCTestCase {
    private var crypto: EEA!
    private var io: FileIO!
    private var keygen: Keygen!
    private var passed: Bool!

    /// Override the setUpWithError to run before each test
    override func setUpWithError() throws {
        try super.setUpWithError()
        crypto = EEA()
        io = FileIO()
        keygen = Keygen()
        passed = true
    }

    /// Override the tearDownWithError to run after each test
    override func tearDownWithError() throws {
        try super.tearDownWithError()
        crypto = nil
        io = nil
        keygen = nil
        passed = true
    }

    /// Test case to make sure key generation works
    func testKeygen() throws {
        var keys: [String]
        do {
            keys = try keygen.genKeys()
            try crypto.keyCheck(keys)
        } catch (let e) {
            throw e
        }
        XCTAssertTrue(passed)
    }

    /// Test case to make sure encrypting and decrypting text works
    func testStringEncrypt() throws {
        var keys: [String]
        do {
            keys = try keygen.genKeys()
        } catch (let e) {
            throw e
        }

        let ogString: String = "This is a test string.\nIt will be encrypted."
        guard let crypt = crypto.encryptText(text: ogString, keys: keys) else {
            throw TestError.nilValue("Encrypting text failed")
        }
        guard let decrypt = crypto.decryptText(text: crypt, keys: keys) else {
            throw TestError.nilValue("Decrypting text failed")
        }
        passed = decrypt == ogString
        XCTAssertTrue(passed)
    }

    /// Test case to make sure encrypting and decrypting files works
    func testFileEncrypt() throws {
        var keys: [String]
        do {
            keys = try keygen.genKeys()
        } catch (let e) {
            throw e
        }

        let ogString: String = "This is a test string.\nIt will be encrypted."

        let data: [UInt8] = Array(ogString.utf8)
        _ = try? io.writeFile(data, filename: "output.txt")
        _ = try? crypto.encryptFile(inFile: "output.txt", keys: keys)
        _ = try? crypto.decryptFile(inFile: "output.txt.eea", keys: keys)
        let tmp = try? io.readFile("output.txt")
        passed = passed && (tmp == data)
        XCTAssertTrue(passed)

        var tmpStr: String? = crypto.encryptText(text: ogString, keys: keys)
        tmpStr = crypto.decryptText(text: tmpStr ?? "", keys: keys)
        passed = passed && (tmpStr == ogString)
        XCTAssertTrue(passed)
    }
}
