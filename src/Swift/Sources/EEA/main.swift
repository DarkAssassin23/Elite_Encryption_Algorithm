import Crypto

func basicTest() -> Bool {
    let crypto = EEA()
    let io = FileIO()
    let keygen = Keygen()
    var passed: Bool = true

    guard let keys = try? keygen.genKeys() else {
        return false
    }

    printKeys(keys: keys)
    let ogString: String = "This is a test string.\nIt will be encrypted."

    let data: [UInt8] = Array(ogString.utf8)
    _ = try? io.writeFile(data, filename: "output.txt")
    _ = try? crypto.encryptFile(inFile: "output.txt", keys: keys)
    _ = try? crypto.decryptFile(inFile: "output.txt.eea", keys: keys)
    let tmp = try? io.readFile("output.txt")
    passed = passed && (tmp == data)
    print("Decrypt success: \(passed)")

    var tmpStr: String? = crypto.encryptText(text: ogString, keys: keys)
    print(tmpStr ?? "N/A")
    tmpStr = crypto.decryptText(text: tmpStr ?? "", keys: keys)
    print(tmpStr ?? "N/A")
    passed = passed && (tmpStr == ogString)
    print("Decrypt success: \(passed)")

    return passed
}

func main() async {
    //print(basicTest() ? "Success!" : "Fail...")
    loadConfig()
    let userIn = UserInput()
    while true {
        let ret = userIn.mainMenu()
        if ret == -1 {
            break
        } else if ret == 0 {
            print("Invalid selection.")
        } else {
            await userIn.submenu(menu: ret)
        }
    }
    print("goodbye.")
}

await main()
