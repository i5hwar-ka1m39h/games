# Rulers of the Sky

A 2D shoot-'em-up built with [Ebitengine](https://ebitengine.org/) (Go 1.25+).

## Build

### Linux

```
go build -o rulers-of-the-sky .
```

Requires: `libgl1-mesa-dev`, `libxcursor-dev`, `libxrandr-dev`, `libxinerama-dev`, `libxi-dev`, `libxxf86vm-dev` (install via your package manager).

### macOS

```
go build -o rulers-of-the-sky .
```

No extra dependencies. The binary bundles an `.app` bundle if you want distribution — use `go build -o RulersOfTheSky.app` and structure it manually, or wrap with `macapp`.

### Windows

```
GOOS=windows GOARCH=amd64 go build -o rulers-of-the-sky.exe .
```

Cross-compile from Linux/macOS with `CGO_ENABLED=1` and a MinGW cross-compiler:

```
sudo apt install gcc-mingw-w64-x86-64
GOOS=windows GOARCH=amd64 CGO_ENABLED=1 CC=x86_64-w64-mingw32-gcc go build -o rulers-of-the-sky.exe .
```

### Web (Wasm)

Build the game binary:

```
GOOS=js GOARCH=wasm go build -o rulers-of-the-sky.wasm .
```

Serve it by copying the wasm binary and the Ebitengine Wasm support script into a directory and serving with any HTTP server:

```
cp "$(go env GOROOT)/misc/wasm/wasm_exec.js" .
# write an index.html that loads wasm_exec.js and rulers-of-the-sky.wasm
python3 -m http.server 8080
```

A minimal `index.html`:

```html
<!DOCTYPE html>
<script src="wasm_exec.js"></script>
<script>
const go = new Go();
WebAssembly.instantiateStreaming(fetch("rulers-of-the-sky.wasm"), go.importObject)
  .then(result => go.run(result.instance));
</script>
```

### Android

Install [gomobile](https://pkg.go.dev/golang.org/x/mobile/cmd/gomobile) and the Android NDK, then:

```
gomobile bind -target=android -androidapi 24 -o rulers-of-the-sky.aar .
```

For a standalone APK, wrap the .aar in an Android project with an `org.fmod.RulersOfTheSky` activity that launches the game.

### iOS

```
gomobile bind -target=ios -o rulers-of-the-sky.framework .
```

Wrap in an Xcode project. Note that `gomobile` requires macOS and Xcode.

## Run (development)

```
go run .
```

Controls: Arrow keys to move, Space to shoot.
