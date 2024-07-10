import Crypto
import Foundation

enum KeygenError: Error {
    case randomBytes(String)
    case keySize(String)
}

#if (!os(macOS) && !os(iOS) && !os(watchOS) && !os(tvOS))
    extension Data {
        /// Returns random data
        ///
        /// - Parameter length: Length of the data in bytes.
        /// - Returns: Generated data of the specified length.
        static func random(length: Int) throws -> Data {
            return Data(
                (0..<length).map { _ in
                    UInt8.random(in: UInt8.min...UInt8.max)
                })
        }
    }
#endif

/// Class to handle the generation of keys
public struct Keygen {

    /// Generate random data to and return the result as a base64 string
    /// - Returns: Random data encoded in a base64 string
    private func genRandBytes() throws -> String {
        #if (os(macOS) || os(iOS) || os(watchOS) || os(tvOS))
            var bytes = [UInt8](repeating: 0, count: SHA256.byteCount)
            let status = SecRandomCopyBytes(
                kSecRandomDefault, bytes.count, &bytes)

            if status == errSecSuccess {
                return Data(bytes).base64EncodedString()
            } else {
                throw KeygenError.randomBytes(
                    "Generating random bytes failed.")
            }
        #else
            guard
                let dataStr = try? Data.random(length: SHA256.byteCount)
                    .base64EncodedString()
            else {
                throw KeygenError.randomBytes(
                    "Generating random bytes failed.")
            }
            return dataStr
        #endif
    }

    /// Generate a set of keys for EEA
    /// - Parameters:
    ///   - size: The hash size of the keys to generate (e.g. 256 = SHA256)
    ///           (default: 256)
    ///   - num: The number of keys to generate (default: 3)
    /// > Note: The `size` **must** be a multiple of 256.
    ///   I.e. 256, 512, 1024, etc.
    ///
    /// - Returns: The list of keys
    public func genKeys(size: Int = 256, num: Int = 3) throws -> [String] {
        if size % 256 != 0 {
            throw KeygenError.keySize(
                "Invalid keysize. Must be a multiple of 256."
            )
        }

        var keys: [String] = [String](repeating: "", count: num)
        for x in (0...num - 1) {
            var currSize: Int = 0
            var key: String = ""
            while currSize < size {
                guard let str = try? genRandBytes() else {
                    throw KeygenError.randomBytes(
                        "Generating random bytes failed.")
                }

                if size - currSize > 256 {
                    let hash = SHA512.hash(data: Data(str.utf8))
                    key += hash.description.split(separator: " ").last!
                    currSize += 512
                } else {
                    let hash = SHA256.hash(data: Data(str.utf8))
                    key += hash.description.split(separator: " ").last!
                    currSize += 256
                }
            }
            keys[x] = key
        }
        return keys
    }
}
