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
                })
        }
    #endif
    // ref: https://stackoverflow.com/a/40089462
    func hexString() -> String {
        return self.map { String(format: "%02hhx", $0) }.joined()
    }
}

/// Generate random data to and return the result as a base64 string
/// - Returns: Random data encoded in a base64 string
func genRandBytes(count: Int = SHA256.byteCount, encode: Bool = true) throws
    -> String
{
    #if (os(macOS) || os(iOS) || os(watchOS) || os(tvOS))
        var bytes = [UInt8](repeating: 0, count: count)
        let status = SecRandomCopyBytes(
            kSecRandomDefault, bytes.count, &bytes)

        if status == errSecSuccess {
            return encode
                ? Data(bytes).base64EncodedString() : Data(bytes).hexString()
        } else {
            throw KeygenError.randomBytes(
                "Generating random bytes failed.")
        }
    #else
        guard let data = try? Data.random(length: count)
        else {
            throw KeygenError.randomBytes(
                "Generating random bytes failed.")
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
