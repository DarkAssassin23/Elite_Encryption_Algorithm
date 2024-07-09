default: build

build:
	swift build

update: 
	swift package update

release:
	swift build -c release

rund: build
	@./.build/debug/EEA

run: release
	@./.build/release/EEA

clean:
	$(RM) -r .build