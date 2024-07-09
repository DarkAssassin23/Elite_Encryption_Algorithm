// swift-tools-version:5.10

import PackageDescription

let package = Package(
    name: "eea",
    products: [
        .executable(name: "EEA", targets: ["EEA"])
    ],
    dependencies: [
        .package(url: "https://github.com/apple/swift-crypto.git", .upToNextMajor(from: "3.5.0"))
    ],
    targets: [
        .executableTarget(
            name: "EEA",
            dependencies: [
              .product(name: "Crypto", package: "swift-crypto")
            ]
        ),
    ]
)