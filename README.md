# ed25519-edsrp
Patches and build system for DJB's ref10 Ed25519 implementation. Used in EdSRP's reference implementation.

## Building
```sh
# Download and patch ref10 sources
make prepare

# Build static and shared libraries (release target)
make

# Build individual targets
make ed25519 # Static lib
make so # Shared lib
make wasm # WebAssembly lib (Emscripten toolchain is required)

# Build all individual targets
make all
```

## Installation
```sh
# Run tests before installing
make test

# Install with PREFIX=/usr/local
make install

# Install with PREFIX=/usr (read below)
make install PREFIX=/usr
```

### Installation Prefix Note
Installation sets `PREFIX=/usr/local` by default. **Unless you add `/usr/local/lib` to `LD_LIBRARY_PATH`, linkers will not be able to find libedsrp_ed25519 on most Linux distros.** Two solutions to this problem exist:
1. Add `/usr/local/lib` to `LD_LIBRARY_PATH`. This addition can be accomplished at a system level by editing `/etc/ld.so.conf`, or at a user level by editing your login shell's rc file (e.g `~/.bashrc`).
2. Install using `make install PREFIX=/usr` to ensure archive and shared object files are placed in `/usr/lib`.

Note that installing under `/usr` instead of `/usr/local` introduces the risk of conflicts with future system packages. However, these conflicts should be avoidable by uninstalling via `make uninstall PREFIX=/usr` *before* installing via your system's package manager.

## Uninstallation/Removal
```sh
# Uninstall from /usr/local
make uninstall

# Uninstall from /usr
make uninstall PREFIX=/usr
```
