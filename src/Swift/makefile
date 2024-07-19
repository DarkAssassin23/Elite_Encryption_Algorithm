default: build

build:
	swift build

update: 
	swift package update

debug: build
	@cp .build/debug/EEA ./EEAd

release:
	swift build -c release
	@cp .build/debug/EEA .

rund: build
	@./.build/debug/EEA

run: release
	@./.build/release/EEA

clean:
	$(RM) -r .build
