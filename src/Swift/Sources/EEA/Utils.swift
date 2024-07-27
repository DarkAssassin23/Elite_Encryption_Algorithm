import Crypto
import Foundation

extension Data {
    #if (!os(macOS) && !os(iOS) && !os(watchOS) && !os(tvOS))
        /// Returns random data
        /// - Parameter length: Length of the data in bytes.
        /// - Returns: Generated data of the specified length.
        static func random(length: Int) throws -> Data {
            return Data(
                (0..<length).map { _ in
                    UInt8.random(in: UInt8.min...UInt8.max)
                }
            )
        }
    #endif
    /// Convert the data to a hexadecimal string
    /// - ref: https://stackoverflow.com/a/40089462
    /// - Returns: The data has a hexadecimal string
    func hexString() -> String {
        return self.map { String(format: "%02hhx", $0) }.joined()
    }
}

/// Generate random data to and return the result as a base64 string
/// - Parameters:
///   - count: The number of bytes to generate
///   - encode: Should the result be encoded in base64
/// - Returns: Random data
func genRandBytes(count: Int = SHA256.byteCount, encode: Bool = true) throws
    -> String
{
    #if (os(macOS) || os(iOS) || os(watchOS) || os(tvOS))
        var bytes = [UInt8](repeating: 0, count: count)
        let status = SecRandomCopyBytes(
            kSecRandomDefault,
            bytes.count,
            &bytes
        )

        if status == errSecSuccess {
            return encode
                ? Data(bytes).base64EncodedString() : Data(bytes).hexString()
        } else {
            throw KeygenError.randomBytes(
                "Generating random bytes failed."
            )
        }
    #else
        guard let data = try? Data.random(length: count)
        else {
            throw KeygenError.randomBytes(
                "Generating random bytes failed."
            )
        }
        return encode ? data.base64EncodedString() : data.hexString()
    #endif
}

/// Given a list of keys, print them out
/// - Parameter keys: The list of keys to print
func printKeys(keys: [String]) {
    for i in (0..<keys.count) {
        print("\(i + 1). \(keys[i])")
    }
}

/// Generate the default config file
func generateDefaultConfig() {
    let fileIO = FileIO()
    let cfg: String = """
        # Elite Encryption Algorithm (EEA) config file

        # The directory to search for your '.keys' files in.
        # NOTE: The default is the same directory as the EEA executable
        # keysDir: ~/.eeaKeys

        """
    _ = try? fileIO.writeFile(Array(cfg.utf8), filename: Globals.cfgFile)
}

/// Create the directory to store the .keys files
/// - Parameter path: The path to create
func makeKeysDir(_ path: String) -> Bool {
    print("The directory, \(path), does not exist.")
    print("Would you like to create it? (y/n) (default: y): ", terminator: "")
    let input = readLine()
    if input == "n" {
        print("Ok. Using default.")
        return false
    }

    do {
        print("Trying to create: \(path)")
        try FileManager.default.createDirectory(
            at: URL(fileURLWithPath: path),
            withIntermediateDirectories: true
        )
    } catch (let e) {
        print("The following error occured when trying to create '\(path)':")
        print(e)
        return false
    }
    print("Success!")
    return true
}

/// Load the config file to set the directory to set environment variables
func loadConfig() {
    let fileIO = FileIO()
    guard let data = try? fileIO.readFile(Globals.cfgFile) else {
        if !fileIO.doesExist(path: Globals.cfgFile) {
            generateDefaultConfig()
        }
        return
    }
    guard let cfg = String(bytes: data, encoding: .utf8) else {
        return
    }
    for line in cfg.split(separator: "\n") {
        let trimmed = line.trimmingCharacters(in: .whitespacesAndNewlines)
        if trimmed.hasPrefix("#") || trimmed.hasPrefix("//") {
            continue
        } else if trimmed.hasPrefix("keysDir:") {
            let index = trimmed.firstIndex(of: ":")!
            let value = trimmed[trimmed.index(after: index)..<trimmed.endIndex]
            var path = value.trimmingCharacters(in: .whitespacesAndNewlines)
            path = path.replacingOccurrences(
                of: "~/",
                with: FileManager.default.homeDirectoryForCurrentUser.path
                    + "/"
            )
            if fileIO.doesExist(path: path) {
                Globals.keysDir = path.hasSuffix("/") ? path : path + "/"
            } else if makeKeysDir(path) {
                Globals.keysDir = path.hasSuffix("/") ? path : path + "/"
            }
        }
    }
}

/// Load the keys from the given keys file
/// - Parameters:
///   - filename: The name of the keys file to load the keys from
///   - password: The password used to decrypt the keys
/// - Returns: List of keys from the keys file
func loadKeys(_ filename: String, _ password: String) throws -> [String] {
    let fileIO = FileIO()
    let eea = EEA()

    let keysData = try fileIO.readFile(filename)
    do {
        guard let dataString = String(bytes: keysData, encoding: .utf8)
        else {
            print("Error converting keys data")
            return []
        }

        var cipherData: [UInt8] = keysData
        let decode = try? eea.decode(data: dataString)
        if let data = decode {
            cipherData = data
        }
        for _ in (0..<Globals.passwordRounds) {
            cipherData = eea.decrypt(
                data: cipherData,
                keys: [password]
            )
        }
        let keyStr = String(bytes: cipherData, encoding: .utf8)
        var keys = Array(
            keyStr!.split(separator: "\n").map { String($0) }
        )
        try eea.keyCheck(keys)

        // Remove salt
        keys.removeFirst()
        return keys
    } catch (let e) {
        throw e
    }
}

#if os(Windows)
    /// Implementation of getpass for use on Windows
    /// - Parameter prompt: The password prompt to display to the user
    /// - Returns: The password
    func getpass(_ prompt: String) -> [CChar]? {
        let star = [UInt8]("*".utf8)
        var password: String = String()

        print(prompt, terminator: "")
        while true {
            let c = _getch()
            let ch = String(bytes: [UInt8(c)], encoding: .utf8)!
            if ch == "\r" || ch == "\n" {
                print()
                break
            } else {
                _putch(Int32(star[0]))
                password += ch
            }
        }
        return password.cString(using: .utf8)
    }
#endif
