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

    enum EncryptDecryptOpts: UInt8 {
        case file = 1
        case dir = 2
        case text = 3
    }

    enum PasswordError: Error {
        case nilPassword(String)
        case passwordMismatch(String)
    }

    enum LoadKeysError: Error {
        case noKeysFiles(String)
        case invalid(String)
    }

    public init() {
        self.keySizes = [256, 512, 1024, 2048]
        self.mainMenuOpts = ["Manage Keys", "Encrypt", "Decrypt"]
        self.keyMenuOpts = ["Add Keys", "Delete Keys", "View Keys"]
        self.eea = EEA()
        self.keygen = Keygen()
        self.fileIO = FileIO()
        self.defaultKeysFile = "keys.keys"

        self.hasKeys = !fileIO.getFilesOfType(ext: ".keys").isEmpty
    }

    /*============================================*/
    /*                  Private                   */
    /*============================================*/

    /*=========================*/
    /*    Helper Functions     */
    /*=========================*/
    /// Print out the menu
    /// - Parameters:
    ///   - menu: The list of menu options
    ///   - msg: The message to display for the menu
    ///   - def: The default menu option if the user just hits enter
    private func printMenu(menu: [String], msg: String? = nil, def: Int = -1) {
        print(msg ?? "Select one of the following options:")
        for i in (0..<menu.count) {
            print("\(i + 1). \(menu[i])")
        }
        if menu.count > 1 {
            print("(1-\(menu.count)) or 'q' to quit", terminator: "")
        } else {
            print("(1) or 'q' to quit", terminator: "")
        }
        if def > 0 {
            print(" (default \(def))", terminator: "")
        }
        print(": ", terminator: "")

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

    /// Display the menu for encrypting/decrypting
    /// - Parameter encrypt: Is this the encryption menu
    private func printEncryptDecryptMenu(encrypt: Bool) {
        let type: String = encrypt ? "Encrypt" : "Decrypt"
        let menu = ["\(type) file", "\(type) directory", "\(type) text"]
        printMenu(menu: menu, def: 1)
    }

    /// Prompt the user if they would like to use Ghost Mode
    /// - Parameter encrypt: Is this for encryption
    /// - Returns: Whether to use Ghost Mode
    private func promptGhostMode(encrypt: Bool) -> Bool {
        print(
            "Would you like to enable Ghost Mode?",
            "(y/n) (default: n): ", terminator: "")
        guard let input = readLine() else {
            return false
        }
        if input == "y" {
            if encrypt {
                print(
                    "WARNING: You are about to encrypt the data with one time",
                    "use keys.\nYou will be unable to decrypt the data unless",
                    "you manually copy down\nthe keys after you encrypt your",
                    "data")
            }
            return true
        }
        return false
    }

    /// Prompt the user to enter the keys to use for Ghost Mode decryption
    /// - Returns: The list of keys
    private func getGhostModeKeys() throws -> [String] {
        print("When you finish entering your keys, type 'done' or hit enter.")
        print("Enter your keys below:")
        var keys: [String] = []
        while true {
            print("\(keys.count + 1): ", terminator: "")
            guard let value = readLine() else {
                throw LoadKeysError.invalid("nil value detected.")
            }
            if value == "done" || value == "" {
                do {
                    try eea.keyCheck(keys)
                    return keys
                } catch (let e) {
                    throw e
                }
            }
            do {
                try eea.keyCheck([value])
                keys.append(value)
            } catch (let e) {
                throw e
            }
        }
    }

    /// Prompt the user for which keys file they would like to load in
    /// - Parameter view: Are the keys being viewed only
    /// - Returns: The list of keys from the file
    private func loadKeysFromFile(view: Bool = false) throws -> [String] {
        let type: String = view ? "view" : "use"
        let keyFiles = fileIO.getFilesOfType(ext: ".keys")
        if keyFiles.isEmpty {
            hasKeys = false
            throw LoadKeysError.noKeysFiles("You dont have any keys files.")
        }
        while true {
            printMenu(
                menu: keyFiles,
                msg: "Select which keys file you would like to \(type):")
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
                let keys = try loadKeys(keyFiles[choice - 1], password)
                return keys
            } catch KeyError.invalidLength, KeyError.lengthMismatch, KeyError
                .invalidKey
            {
                var err: String = "Error: Invalid keys detected. "
                err += "Did you enter your password right?"
                throw LoadKeysError.invalid(err)
            } catch (let e) {
                throw e
            }
        }
        return []
    }

    /*=========================*/
    /*  End Helper Functions   */
    /*=========================*/

    /*=========================*/
    /*      Key Submenus       */
    /*=========================*/
    private func createKeys() -> [String]? {
        let size: Int = getDesiredKeySize()
        if size == 0 {
            return nil
        }

        let num: Int = getDesiredNumKeys()
        if num == 0 {
            return nil
        }

        do {
            let k = try keygen.genKeys(size: size, num: num)
            return k
        } catch (let e) {
            print("An error occured trying to generate your keys:\n\(e)")
            return nil
        }
    }
    /// Create new keys for the user
    private func addKeys() {
        guard let keys: [String] = createKeys() else {
            return
        }

        while true {
            print(
                "Enter a filename to save your keys to. It should in in .keys")
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

            if fileIO.doesExist(path: filename) {
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

                for _ in (0..<passwordRounds) {
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
        do {
            let keys = try loadKeysFromFile(view: true)
            printKeys(keys: keys)
        } catch (let e) {
            print("The following error occured when trying to view your keys:")
            print(e)
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
    /*=========================*/
    /*    End Key Submenus     */
    /*=========================*/

    /*=============================*/
    /*    Encrypt/Decrypt Submenu  */
    /*=============================*/
    /// Prompt the user for what keys to use for encryption or decryption
    /// - Parameters:
    ///   - ghost: Encrypting or decrypting with Ghost Mode
    ///   - encrypt: Are we encrypting
    private func keysPrompt(_ ghost: Bool, _ encrypt: Bool) -> [String]? {
        var keys: [String] = []
        if ghost && encrypt {
            guard let k = createKeys() else {
                return nil
            }
            keys = k
        } else if ghost {
            do {
                keys = try getGhostModeKeys()
            } catch (let e) {
                print(e)
                return nil
            }
        } else {
            if !hasKeys {
                noKeysWarning()
            }
            // User didn't add keys
            if !hasKeys {
                return nil
            }
            do {
                keys = try loadKeysFromFile(view: false)
            } catch (let e) {
                print(
                    "The following error occured when trying to load",
                    "your keys:")
                print(e)
                return nil
            }
        }
        return !keys.isEmpty ? keys : nil
    }

    /// Prompt the user if the file(s) should be overwritten
    /// - Parameter dir: Are we overwritting a directory
    private func shouldOverwrite(dir: Bool = false) -> Bool? {
        let type: String = dir ? "directory contents" : "file"
        var overwrite: Bool = true
        print("Overwrite the \(type)? (y/n) (default: y): ", terminator: "")
        guard let input = readLine() else {
            print("Error: nil value detected")
            return nil
        }
        if input == "n" {
            overwrite = false
        }
        return overwrite
    }

    /// Prompt the user for the file or directory to encrypt/decrypt
    /// - Parameters:
    ///   - encrypt: Are we encrypting
    ///   - dir: Are we working with a directory
    private func getTarget(_ encrypt: Bool, dir: Bool = false) -> String? {
        let type: String = encrypt ? "encrypt" : "decrypt"
        let target: String = dir ? "directory" : "file"
        print(
            "Enter the name of the \(target) you would like to \(type): ",
            terminator: "")
        guard let input = readLine() else {
            print("Error: nil value detected for the \(target).")
            return nil
        }
        if input == "" {
            print("Error: No \(target) specified.")
            return nil
        }
        if !fileIO.doesExist(path: input) {
            print("Error: the \(target), '\(input)', does not exist.")
            return nil
        }
        return input
    }

    /// Handle encryption and decryption of a file
    /// - Parameters:
    ///   - ghost: Encrypting or decrypting with Ghost Mode
    ///   - encrypt: Are we encrypting
    private func encryptDecryptFile(ghost: Bool, encrypt: Bool) {
        let type: String = encrypt ? "Encryption" : "Decryption"
        guard let filename = getTarget(encrypt) else {
            return
        }
        guard let overwrite = shouldOverwrite() else {
            return
        }
        guard let keys: [String] = keysPrompt(ghost, encrypt) else {
            print("Aborting...")
            return
        }

        do {
            if encrypt {
                let outfile: String? = overwrite ? nil : filename + ".eea"
                _ = try eea.encryptFile(
                    inFile: filename, keys: keys,
                    outFile: outfile)
            } else {
                if overwrite {
                    _ = try eea.decryptFile(inFile: filename, keys: keys)
                } else {
                    // Remove .eea extension
                    let index = filename.lastIndex(of: ".")!
                    let outfile = String(filename[filename.startIndex..<index])
                    _ = try eea.decryptFile(
                        inFile: filename, keys: keys,
                        outFile: outfile)
                }
            }
        } catch (let e) {
            print("\(type) failed with the following error:")
            print(e)
            return
        }
        if ghost && encrypt {
            print("The file was encrypted with the following keys:")
            printKeys(keys: keys)
        }
        print("\(type) success: \(filename)")
    }

    /// Handle encryption and decryption of a directory
    /// - Parameters:
    ///   - ghost: Encrypting or decrypting with Ghost Mode
    ///   - encrypt: Are we encrypting
    private func encryptDecryptDir(ghost: Bool, encrypt: Bool) {

    }

    /// Handle encryption and decryption of text
    /// - Parameters:
    ///   - ghost: Encrypting or decrypting with Ghost Mode
    ///   - encrypt: Are we encrypting
    private func encryptDecryptText(ghost: Bool, encrypt: Bool) {
        let operation: String = encrypt ? "encrypt" : "decrypt"
        let type: String = encrypt ? "Encryption" : "Decryption"
        let closing: String = "</done>"
        var text: String = String()

        print("When you finish entering your text, type: \(closing)")
        print("Enter the text you would like to \(operation):")
        while true {
            guard let input = readLine() else {
                print("Error: Failed to read line. Aborting...")
                return
            }
            if input.hasSuffix(closing) {
                // Make sure there isn't any text that needs to be added
                if input != closing {
                    // Remove closing
                    let index = input.lastIndex(of: "<")!
                    text += String(input[input.startIndex..<index])
                }
                break
            }
            text += input
        }
        if text.isEmpty {
            print("No text provided. Nothing to do")
            return
        }

        guard let keys: [String] = keysPrompt(ghost, encrypt) else {
            print("Aborting...")
            return
        }

        guard
            let output = encrypt
                ? eea.encryptText(text: text, keys: keys)
                : eea.decryptText(text: text, keys: keys)
        else {
            return
        }

        if ghost && encrypt {
            print("The text was encrypted with the following keys:")
            printKeys(keys: keys)
        }
        print("\(type) success!")
        print(output)
    }

    /// Submenu to handle encryption and decryption
    /// - Parameters:
    ///   - ghost: Encrypting or decrypting with Ghost Mode
    ///   - encrypt: Are we encrypting
    private func encryptDecryptSubmenu(ghost: Bool, encrypt: Bool) {
        while true {
            printEncryptDecryptMenu(encrypt: encrypt)
            let input = readLine()
            var choice: Int
            if let c = Int(input ?? "N/A") {
                choice = c
            } else {
                if input == "q" {
                    return
                } else if input == "" {
                    choice = 1
                } else {
                    choice = 0
                }
            }
            guard let opt = EncryptDecryptOpts(rawValue: UInt8(choice))
            else {
                print("Invalid selection.")
                continue
            }
            switch opt {
            case EncryptDecryptOpts.file:
                encryptDecryptFile(ghost: ghost, encrypt: encrypt)
                return
            case EncryptDecryptOpts.dir:
                encryptDecryptDir(ghost: ghost, encrypt: encrypt)
                return
            case EncryptDecryptOpts.text:
                encryptDecryptText(ghost: ghost, encrypt: encrypt)
                return
            }
        }
    }
    /*=============================*/
    /* End Encrypt/Decrypt Submenu */
    /*=============================*/

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

    /// Go to the submenu the user selected
    /// - Parameter menu: The ID of the submenu to go to
    public func submenu(menu: Int) {
        guard let choice = MenuOpts(rawValue: UInt8(menu)) else {
            return
        }
        switch choice {
        case MenuOpts.keys:
            keysSubmenu()
            break
        case MenuOpts.encrypt:
            let ghost = promptGhostMode(encrypt: true)
            encryptDecryptSubmenu(ghost: ghost, encrypt: true)
            break
        case MenuOpts.decrypt:
            let ghost = promptGhostMode(encrypt: false)
            encryptDecryptSubmenu(ghost: ghost, encrypt: false)
            break
        }
    }
}
