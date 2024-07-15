import Foundation

enum FileIOError: Error {
    case readFailed(String)
    case writeFailed(String)
    case noData(String)
}

/// Class to handle file reads and writes
public struct FileIO {
    private let fileManager = FileManager.default

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

    /// Delete the specified file
    /// - Parameter filename: The file to be deleted
    /// - Returns: True if the file was deleted successfully
    public func deleteFile(filename: String) -> Bool {
        do {
            try fileManager.removeItem(atPath: filename)
            return true
        } catch {
            return false
        }
    }

    /// Check if the given file exists
    /// - Parameter filename: The name of the file to check
    /// - Returns: If the given file exists
    public func doesExist(filename: String) -> Bool {
        return fileManager.fileExists(atPath: filename)
    }

    /// Return a list of all the files with a given extension
    /// - Parameter ext: The file extension of files to look for
    /// - Returns: A list of files with the given extension
    public func getFilesOfType(ext: String) -> [String] {
        guard let conts = try? fileManager.contentsOfDirectory(atPath: ".")
        else {
            return []
        }
        var files: [String] = []
        for f in conts {
            if f.hasSuffix(ext) {
                files.append(f)
            }
        }
        return files
    }
}
