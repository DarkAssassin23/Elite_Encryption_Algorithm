/// Given a list of keys, print them out
/// - Parameter keys: The list of keys to print
func printKeys(keys: [String]) {
    for i in (0..<keys.count) {
        print("\(i + 1). \(keys[i])")
    }
}
