func main() {
    let crypto = EEA()
    let io = FileIO()
    let keygen = Keygen()
    guard let keys = try? keygen.genKeys(size: 256) else {
        return
    }

    for i in (0...keys.count - 1) {
        print("\(i + 1). \(keys[i])")
    }
    let ogString: String = "This is a test string.\nIt will be encrypted."

    let data: [UInt8] = Array(ogString.utf8)
    _ = try? io.writeFile(data, filename: "output.txt")
    _ = try? crypto.encryptFile(inFile: "output.txt", keys: keys)
    _ = try? crypto.decryptFile(inFile: "output.txt.eea", keys: keys)
    let tmp = try? io.readFile("output.txt")
    print("Decrypt success: \(tmp == data)")

    var tmpStr: String? = crypto.encryptText(text: ogString, keys: keys)
    print(tmpStr ?? "N/A")
    tmpStr = crypto.decryptText(text: tmpStr ?? "", keys: keys)
    print(tmpStr ?? "N/A")
    print("Decrypt success: \(tmpStr == ogString)")
}

main()