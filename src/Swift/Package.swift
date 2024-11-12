// swift-tools-version:5.4

import PackageDescription

let package = Package(
    name: "eea",
    platforms: [
        .macOS(.v10_15)
    ],
    products: [
        .executable(name: "EEA", targets: ["EEA"])
    ],
    dependencies: dependencies,
    targets: [
        .target(
            name: "EEAUtils",
            dependencies: [
                .product(name: "Crypto", package: "swift-crypto")
            ]
        ),
        .executableTarget(
            name: "EEA",
            dependencies: [
                "EEAUtils"
            ]
        ),
        .testTarget(
            name: "EEATests",
            dependencies: [
                "EEAUtils"
            ]
        ),
    ]
)

func hasEnvironmentVariable(_ name: String) -> Bool {
    return ProcessInfo.processInfo.environment[name] != nil
}

var useLocalDependencies: Bool {
    hasEnvironmentVariable("SWIFTCLI_USE_LOCAL_DEPS")
}
var dependencies: [Package.Dependency] {
    if useLocalDependencies {
        return [
            .package(path: "../swift-crypto")
        ]
    } else {
        return [
            .package(
                url: "https://github.com/apple/swift-crypto.git",
                from: "3.5.0"
            )
        ]
    }
}
