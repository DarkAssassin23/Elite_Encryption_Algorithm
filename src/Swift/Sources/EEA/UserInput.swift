public struct UserInput {
    private let keySizes: [Int]
    private let mainMenuOpts: [String]
    private let keyMenuOpts: [String]
    private let keygen: Keygen
    private let fileIO: FileIO
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

    public init() {
        self.keySizes = [256, 512, 1024, 2048]
        self.mainMenuOpts = ["Manage Keys", "Encrypt", "Decrypt"]
        self.keyMenuOpts = ["Add Keys", "Delete Keys", "View Keys"]
        self.keygen = Keygen()
        self.fileIO = FileIO()
    }

    /// Print out the menu
    /// - Parameter menu: The list of menu options
    private func printMenu(menu: [String]) {
        print("Select one of the following options:")
        for i in (0..<menu.count) {
            print("\(i + 1). \(menu[i])")
        }
        print("(1-\(menu.count)) or 'q' to quit: ", terminator: "")
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
        let size: Int = getDesiredKeySize()
        if size == 0 {
            return
        }

        let num: Int = getDesiredNumKeys()
        if num == 0 {
            return
        }

        do {
            let keys = try keygen.genKeys(size: size, num: num)
            for i in (0..<keys.count) {
                print("\(i + 1). \(keys[i])")
            }
        } catch (let e) {
            print("An error occured trying to generate your keys:\n\(e)")
        }
    }

    /// Submenu for managing a users keys
    private func keysSubmenu() {
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
        }
        guard let choice = KeyOpts(rawValue: UInt8(selection)) else {
            return
        }
        switch choice {
        case KeyOpts.add:
            addKeys()
            break
        default:
            break
        }
    }

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
                "(1-\(keySizes.count + 1)) or 'q' to quit ",
                "(default: \(defaultSelection)): ",
                terminator: "")

            let input = readLine()
            guard let choice = Int(input ?? "N/A") else {
                if input == "q" {
                    break
                }
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
                "Enter the number of keys to generate, or 'q' to quit ",
                "(default: \(defaultSelection)): ",
                terminator: "")
            let input = readLine()
            guard let n = Int(input ?? "N/A") else {
                if input == "q" {
                    break
                }
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
