default: build

build:
	swift build

update: 
	swift package update

debug: build
	@cp .build/debug/EEA ./EEAd

release:
	swift build -c release
	@strip .build/release/EEA
	@cp .build/release/EEA .

rund: build
	@./.build/debug/EEA

run: release
	@./.build/release/EEA

test:
	swift test --parallel

clean:
	$(RM) -r .build
