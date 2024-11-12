import Foundation

struct Globals {
    private let path: String = Bundle.main.bundlePath
    static let passwordRounds: UInt8 = 5
    static let cfgFile: String = Bundle.main.bundlePath + "/eea.conf"
    static var keysDir: String = Bundle.main.bundlePath + "/"
}
