# c2pconvert
c2pconvert is a command-line tool for converting Casio Classpad II images (C2P format) to PNGs.

## Usage
```
c2pconvert input.c2p output.png
```

## Building from source
Building from source requires libpng-dev and zlib1g-dev.
```
git clone https://github.com/AymenQ/c2pconvert
cd c2pconvert
cmake .
make
```

## Contributing
Pull requests are welcome. If you discover a bug, please open an issue.

## License
This project is licensed under the [MIT license](/LICENSE.txt).

This project makes use of [cxxopts](https://github.com/jarro2783/cxxopts) (included in the repository), as well as [zlib](https://zlib.net) & [libpng](http://www.libpng.org/pub/png/libpng.html).
