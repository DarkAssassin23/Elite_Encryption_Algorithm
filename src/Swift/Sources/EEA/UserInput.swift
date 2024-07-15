import Crypto
import Foundation

/// Class to handle the user input portions of the program
public class UserInput {
    private let keySizes: [Int]
    private let mainMenuOpts: [String]
    private let keyMenuOpts: [String]
    private let eea: EEA
    private let keygen: Keygen
    private let fileIO: FileIO
    private let defaultKeysFile: String
    private let rounds: UInt8

    private var hasKeys: Bool
    enum MenuOpts: UInt8 {
        case keys = 1
        case encrypt = 2
        case decrypt = 3
    }

    enum KeyOpts: UInt8 {
        case add = 1
        case del = 2
        case view = 3
    }

    enum PasswordError: Error {
        case nilPassword(String)
        case passwordMismatch(String)
    }

    public init() {
        self.keySizes = [256, 512, 1024, 2048]
        self.mainMenuOpts = ["Manage Keys", "Encrypt", "Decrypt"]
        self.keyMenuOpts = ["Add Keys", "Delete Keys", "View Keys"]
        self.eea = EEA()
        self.keygen = Keygen()
        self.fileIO = FileIO()
        self.defaultKeysFile = "keys.keys"
        self.rounds = 5

        self.hasKeys = !fileIO.getFilesOfType(ext: ".keys").isEmpty
    }

    /*============================================*/
    /*                  Private                   */
    /*============================================*/
    /// Print out the menu
    /// - Parameter menu: The list of menu options
    private func printMenu(menu: [String], msg: String? = nil) {
        print(msg ?? "Select one of the following options:")
        for i in (0..<menu.count) {
            print("\(i + 1). \(menu[i])")
        }
        if menu.count > 1 {
            print("(1-\(menu.count)) or 'q' to quit: ", terminator: "")
        } else {
            print("(1) or 'q' to quit: ", terminator: "")
        }
    }

    /// Prompt the user for their password or to set one up
    /// - Parameter setup: Is the password being set up for the first time
    /// - Returns: The hash of the entered password
    private func getPassword(setup: Bool = true) throws -> String {
        guard let p1 = getpass("Enter your password for they keys file: ")
        else {
            throw PasswordError.nilPassword("Error: nil received for password")
        }
        let password = String(cString: p1)
        if setup {
            guard let p2 = getpass("Re-type password: ") else {
                throw PasswordError.nilPassword(
                    "Error: nil received for password")
            }
            let reenter = String(cString: p2)
            if password != reenter {
                throw PasswordError.passwordMismatch(
                    "Error: Passwords do not match")
            }
        }
        let hash = SHA512.hash(data: Data(password.utf8))
        return String(hash.description.split(separator: " ").last!)
    }

    /// Prompt for a key size that is not one of the pre-sets found in
    /// `keySizes`
    /// - Returns The size each key should be
    private func getManualKeySize() -> Int {
        var size: Int = 0
        repeat {
            print(
                "Enter the size each key should be, or 'q' to quit: ",
                terminator: "")
            let input = readLine()
            guard let s = Int(input ?? "N/A") else {
                if input == "q" {
                    break
                }
                print("Invalid size.")
                continue
            }
            if s < 256 || s % 256 != 0 {
                print(
                    "The given size, \(s), is invalid.",
                    "Size must be a multiple of 256.")
                continue
            }
            size = s
        } while size == 0
        return size
    }

    /// Create new keys for the user
    private func addKeys() {
        var keys: [String] = []
        let size: Int = getDesiredKeySize()
        if size == 0 {
            return
        }

        let num: Int = getDesiredNumKeys()
        if num == 0 {
            return
        }

        do {
            let k = try keygen.genKeys(size: size, num: num)
            keys = k
        } catch (let e) {
            print("An error occured trying to generate your keys:\n\(e)")
            return
        }

        while true {
            print(
                "Enter a filename to save your kesy to. It should in in .keys")
            print(
                "Filename or 'q' to quit (default: \(defaultKeysFile)) ",
                terminator: "")
            var filename: String
            let input = readLine()

            if input == "" {
                filename = defaultKeysFile
            } else if input!.hasSuffix(".keys") {
                filename = input!
            } else if input == "q" {
                return
            } else {
                print("Invalid filename. It should end in .keys")
                continue
            }

            if fileIO.doesExist(filename: filename) {
                print("WARNING the file '\(filename)' already exists.")
                print(
                    "Are you sure you want to override it? (y/n)",
                    "(default: n): ", terminator: "")
                guard let input = readLine() else {
                    continue
                }
                if input != "y" {
                    continue
                }
            }

            do {
                let password = try getPassword()
                var salt = try genRandBytes(
                    count: keys[0].count / 2, encode: false)
                salt += "\n"

                var data: [UInt8] = Array(salt.utf8)
                for key in keys {
                    data.append(contentsOf: Array(key.utf8))
                    data.append(contentsOf: Array("\n".utf8))
                }

                for _ in (0..<rounds) {
                    data = try eea.encrypt(data: data, keys: [password])
                }
                let encryptedKeys = try eea.encode(data: data)
                _ = try fileIO.writeFile(
                    Array(encryptedKeys.utf8),
                    filename: filename)
                printKeys(keys: keys)
            } catch (let e) {
                print("The following error occured when saving your keys:")
                print(e)
            }
            hasKeys = true
            return
        }
    }

    /// List the keys files and print out the keys for the user selects
    private func viewKeys() {
        let keyFiles = fileIO.getFilesOfType(ext: ".keys")
        if keyFiles.isEmpty {
            print("You do not have any keys files.")
            hasKeys = false
            return
        }
        while true {
            printMenu(
                menu: keyFiles,
                msg: "Select which keys file you would like to view:")
            let input = readLine()
            var choice: Int
            if let c = Int(input ?? "N/A") {
                choice = c
            } else {
                if input == "q" {
                    break
                } else if input == "" {
                    choice = 1
                } else {
                    choice = -1
                }
            }
            if choice < 1 || choice > keyFiles.count {
                print("Invalid selection.")
                continue
            }

            do {
                let password = try getPassword(setup: false)
                let keysData = try fileIO.readFile(keyFiles[choice - 1])
                guard let dataString = String(bytes: keysData, encoding: .utf8)
                else {
                    print("Error converting keys data")
                    return
                }

                var cipherData = try eea.decode(data: dataString)
                for _ in (0..<rounds) {
                    cipherData = try eea.decrypt(
                        data: cipherData, keys: [password])
                }
                let keyStr = String(bytes: cipherData, encoding: .utf8)
                var keys = Array(
                    keyStr!.split(separator: "\n").map { String($0) })

                // Remove salt
                keys.removeFirst()
                try eea.keyCheck(keys)
                printKeys(keys: keys)
            } catch KeyError.invalidLength, KeyError.lengthMismatch, KeyError
                .invalidKey
            {
                print(
                    "Error: Invalid keys detected.",
                    "Did you enter your password right?")
            } catch (let e) {
                print("The following error occured reading your keys:")
                print(e)
                return
            }
        }
    }

    /// List the list of keys files and ask the user which they'd like to
    /// delete
    private func deleteKeys() {
        while true {
            let keyFiles = fileIO.getFilesOfType(ext: ".keys")
            if keyFiles.isEmpty {
                print("You do not have any keys files.")
                hasKeys = false
                return
            }
            printMenu(
                menu: keyFiles,
                msg: "Select which keys file you would like to delete:")
            let input = readLine()
            var choice: Int
            if let c = Int(input ?? "N/A") {
                choice = c
            } else {
                if input == "q" {
                    break
                } else if input == "" {
                    choice = 1
                } else {
                    choice = -1
                }
            }
            if choice < 1 || choice > keyFiles.count {
                print("Invalid selection.")
                continue
            }
            print(
                "Are you sure you want to delete \(keyFiles[choice - 1])?",
                "(y/n) (default: n): ", terminator: "")
            guard let input = readLine() else {
                continue
            }
            if input != "y" {
                continue
            }

            if fileIO.deleteFile(filename: keyFiles[choice - 1]) {
                print("Keys file was deleted successfully.")
                if keyFiles.count - 1 == 0 {
                    hasKeys = false
                    return
                }
            } else {
                print("Error: Failed to delete keys file.")
            }
        }
    }

    /// Handle if no keys file exists when opening the keys menu
    private func noKeysWarning() {
        print("No keys file exists.")
        print(
            "Would you like to create one? (y/n) (default: y): ",
            terminator: "")
        let input = readLine()
        if input != "n" {
            addKeys()
        }
    }

    /// Submenu for managing a users keys
    private func keysSubmenu() {
        if !hasKeys {
            noKeysWarning()
        }

        var selection: Int = 0
        while selection == 0 {
            printMenu(menu: keyMenuOpts)
            let input = readLine()
            guard let choice = Int(input ?? "N/A") else {
                if input == "q" {
                    break
                }
                print("Invalid selection.")
                continue
            }
            if choice < 1 || choice > keyMenuOpts.count {
                print("Invalid selection.")
                continue
            }
            selection = choice
            guard let choice = KeyOpts(rawValue: UInt8(selection)) else {
                return
            }
            switch choice {
            case KeyOpts.add:
                addKeys()
                break
            case KeyOpts.del:
                deleteKeys()
                break
            case KeyOpts.view:
                viewKeys()
                break
            }
            selection = 0
        }
    }

    /*============================================*/
    /*                  Public                    */
    /*============================================*/
    /// Prompt the user for how big the keys they want to generate should be
    /// - Returns: The size each key should be
    public func getDesiredKeySize() -> Int {
        var size: Int = 0
        let defaultSelection: Int = 2
        repeat {
            // Note: Not using printMenu due to this special 'other' case
            print("Select the desired key size: ")
            for i in (0..<keySizes.count) {
                print("\(i + 1). \(keySizes[i])-bits")
            }
            print("\(keySizes.count + 1). Other")
            print(
                "(1-\(keySizes.count + 1)) or 'q' to quit",
                "(default: \(defaultSelection)): ",
                terminator: "")

            let input = readLine()
            guard let choice = Int(input ?? "N/A") else {
                if input == "q" {
                    break
                }
                print("Using default selction of \(defaultSelection)")
                return keySizes[defaultSelection - 1]
            }
            if choice < 1 || choice > keySizes.count + 1 {
                print("Invalid selection.")
                continue
            }
            if choice > keySizes.count {
                size = getManualKeySize()
            } else {
                size = keySizes[choice - 1]
            }
        } while size == 0
        return size
    }

    /// Prompt the user for how many keys they want to generate
    /// - Returns: The number of keys the user wants to generate
    public func getDesiredNumKeys() -> Int {
        let defaultSelection: Int = 3
        var num: Int = 0
        repeat {
            print(
                "Enter the number of keys to generate, or 'q' to quit",
                "(default: \(defaultSelection)): ",
                terminator: "")
            let input = readLine()
            guard let n = Int(input ?? "N/A") else {
                if input == "q" {
                    break
                }
                print("Using default value of \(defaultSelection)")
                return defaultSelection
            }
            if n < 1 {
                print("The number of keys to generate must be at least 1")
                continue
            }
            num = n
        } while num == 0
        return num
    }

    /// Print the main menu and return the users selection
    /// - Returns: The users selection
    public func mainMenu() -> Int {
        printMenu(menu: mainMenuOpts)
        let input = readLine()
        guard let choice = Int(input ?? "N/A") else {
            if input == "q" {
                return -1
            }
            return 0
        }

        return (choice >= 1 && choice <= mainMenuOpts.count) ? choice : 0
    }

    public func submenu(menu: Int) {
        guard let choice = MenuOpts(rawValue: UInt8(menu)) else {
            return
        }
        switch choice {
        case MenuOpts.keys:
            keysSubmenu()
            break
        default:
            break
        }
    }
}
