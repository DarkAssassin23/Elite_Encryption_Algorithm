import Crypto
import Foundation

enum KeygenError: Error {
    case randomBytes(String)
    case keySize(String)
}

/// Class to handle the generation of keys
public struct Keygen {
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
                        "Generating random bytes failed."
                    )
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

    /// Prompt the user for the size and number of keys to generate for EEA
    /// - Returns: List of keys to be used for EEA
    public func genUserKeys() throws -> [String] {
        let userIn = UserInput()
        let size: Int = userIn.getDesiredKeySize()
        let num: Int = userIn.getDesiredNumKeys()

        do {
            let keys = try genKeys(size: size, num: num)
            return keys
        } catch (let e) {
            throw e
        }
    }
}
