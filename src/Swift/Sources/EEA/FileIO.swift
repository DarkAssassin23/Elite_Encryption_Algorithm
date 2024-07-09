import Foundation

enum FileIOError: Error {
    case readFailed(String)
    case writeFailed(String)
    case noData(String)
}

/// Class to handle file reads and writes
public struct FileIO {
    /// Read the contents of the file and return the raw data
    /// - Parameters
    ///   - filename: The name of the file to read the data from
    /// - Returns: A byte array containing the raw file data
    public func readFile(_ filename: String) throws -> [UInt8] {
        do {
            let data = try Data(contentsOf: URL(fileURLWithPath: filename))
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
    /// - Returns: If the write was successful
    public func writeFile(_ data: [UInt8]?, filename: String) throws -> Bool {
        guard let data = data else {
            throw FileIOError.noData("Error: The data provided was nil.")
        }
        let content = Data(data)

        do {
            try content.write(to: URL(fileURLWithPath: filename))
            return true
        } catch {
            throw FileIOError.writeFailed(
                "Error: Failed to write to the file \(filename).")
        }
    }
}
