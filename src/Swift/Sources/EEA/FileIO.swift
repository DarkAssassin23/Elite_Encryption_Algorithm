import Foundation  // Data, URL

enum FileIOError: Error {
    case readFailed(String)
    case writeFailed(String)
    case noData(String)
}

public struct FileIO {
    /// Read the contents of the file and return the raw data
    /// - Parameters
    ///   - filename: The name of the file to read the data from
    ///   - encrypted: Is the data in the file we are reading encrypted
    /// - Returns: A byte array containing the raw file data
    public func readFile(_ filename: String, encrypted: Bool = false) throws
        -> [UInt8]
    {
        do {
            let data = try Data(contentsOf: URL(fileURLWithPath: filename))
            if encrypted {
                guard
                    let stream = String(bytes: data, encoding: .utf8)?
                        .fromBase64()
                else {
                    throw Base64Error.encodeFailed(
                        "Error: Encoding data to base64 failed.")
                }
                let contents = Data(Array(stream.utf8))
                return Array(contents)
            }
            return Array(data)
        } catch {
            throw FileIOError.readFailed(
                "Error: Reading the file \(filename) failed.")
        }
    }

    /// Write the raw data to a file
    /// - Parameters:
    ///   - data: The raw data, as a byte array, to be written
    ///   - filename: The name of the file to write the data to
    ///   - encrypted: Is the data encrypted
    /// - Returns: If the write was successful
    public func writeFile(
        _ data: [UInt8]?, filename: String, encrypted: Bool = false
    ) throws -> Bool {
        do {
            guard let data = data else {
                throw FileIOError.noData("Error: The data provided was nil.")
            }
            if encrypted {
                guard
                    let stream = String(bytes: data, encoding: .utf8)?
                        .toBase64()
                else {
                    throw Base64Error.decodeFailed(
                        "Decoding the data from base64 failed.")
                }
                let contents = Data(Array(stream.utf8))
                try contents.write(to: URL(fileURLWithPath: filename))
                return true
            }
            let content = Data(data)
            try content.write(to: URL(fileURLWithPath: filename))
            return true
        } catch {
            throw FileIOError.writeFailed(
                "Error: Failed to write to the file \(filename).")
        }
    }
}
