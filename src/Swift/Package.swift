// swift-tools-version:5.4

import PackageDescription

func hasEnvironmentVariable(_ name: String) -> Bool {
    return ProcessInfo.processInfo.environment[name] != nil
}

var useLocalDependencies: Bool {
    hasEnvironmentVariable("SWIFTCLI_USE_LOCAL_DEPS")
}

#if compiler(>=6.0)
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
                    "EEAUtils",
                    .product(name: "Testing", package: "swift-testing"),
                ]
            ),
        ]
    )

    var dependencies: [Package.Dependency] {
        if useLocalDependencies {
            return [
                .package(path: "./Dependencies/swift-crypto"),
                .package(path: "./Dependencies/swift-testing"),
            ]
        } else {
            return [
                .package(
                    url: "https://github.com/apple/swift-crypto.git",
                    from: "3.5.0"
                ),
                .package(
                    url: "https://github.com/swiftlang/swift-testing.git",
                    from: "600.0.0"
                ),
            ]
        }
    }

#else
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

    var dependencies: [Package.Dependency] {
        if useLocalDependencies {
            return [
                .package(path: "./Dependencies/swift-crypto")
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

#endif
