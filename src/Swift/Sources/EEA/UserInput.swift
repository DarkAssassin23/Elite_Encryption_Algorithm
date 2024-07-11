public struct UserInput {
    private let keySizes: [Int]

    public init() {
        // Pre-set key size options
        self.keySizes = [256, 512, 1024, 2048]
    }

    /// Prompt for a key size that is not one of the pre-sets found in
    /// `keySizes`
    /// - Returns The size each key should be
    private func getManualKeySize() -> Int {
        var size: Int = 0
        repeat {
            print("Enter the size each key should be: ", terminator: "")
            let input = readLine()
            guard let s = Int(input ?? "N/A") else {
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

    /// Prompt the user for how big the keys they want to generate should be
    /// - Returns: The size each key should be
    public func getDesiredKeySize() -> Int {
        var size: Int = 0
        repeat {
            // Print options
            print("Select the desired key size: ")
            for i in (0..<keySizes.count) {
                print("\(i + 1). \(keySizes[i])")
            }
            print("\(keySizes.count + 1). Other")
            print(">> ", terminator: "")

            let input = readLine()
            guard let choice = Int(input ?? "N/A") else {
                print("Invalid selection.")
                continue
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
        var num: Int = 0
        repeat {
            print("Enter the number of keys to generate: ", terminator: "")
            let input = readLine()
            guard let n = Int(input ?? "N/A") else {
                print("Invalid number.")
                continue
            }
            if n < 1 {
                print("The number of keys to generate must be at least 1")
                continue
            }
            num = n
        } while num == 0
        return num
    }
}
