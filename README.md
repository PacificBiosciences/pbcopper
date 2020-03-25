# pbcopper

The **pbcopper** library provides a suite of data structures, algorithms, and utilities
for PacBio C++ applications.

## Getting Started

Download repo & compile library:

```sh
git clone https://github.com/PacificBiosciences/pbcopper.git && cd pbcopper
meson build .
ninja -C build
```

To build & run tests:

```sh
ninja -C build test
```

## Primary API modules

- **PacBio::CLI(v2)** : application interfaces and command line options
- **PacBio::Data** : main data types & data structures
- **PacBio::JSON** : JSON support
- **PacBio::Logging** : logging utilities
- **PacBio::Parallel** : support for running concurrent tasks
- **PacBio::Pbmer** : DeBruijn graphs and kmers
- **PacBio::Utility** : miscellaneous utilities

##  Documentation

  - [Changelog](https://github.com/PacificBiosciences/pbcopper/blob/master/CHANGELOG.md)

## License

 - [PacBio open source license](https://github.com/PacificBiosciences/pbcopper/blob/master/LICENSE.txt)

DISCLAIMER
----------
THIS WEBSITE AND CONTENT AND ALL SITE-RELATED SERVICES, INCLUDING ANY DATA, ARE
PROVIDED "AS IS," WITH ALL FAULTS, WITH NO REPRESENTATIONS OR WARRANTIES OF ANY
KIND, EITHER EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, ANY WARRANTIES
OF MERCHANTABILITY, SATISFACTORY QUALITY, NON-INFRINGEMENT OR FITNESS FOR A
PARTICULAR PURPOSE. YOU ASSUME TOTAL RESPONSIBILITY AND RISK FOR YOUR USE OF THIS
SITE, ALL SITE-RELATED SERVICES, AND ANY THIRD PARTY WEBSITES OR APPLICATIONS.
NO ORAL OR WRITTEN INFORMATION OR ADVICE SHALL CREATE A WARRANTY OF ANY KIND.
ANY REFERENCES TO SPECIFIC PRODUCTS OR SERVICES ON THE WEBSITES DO NOT CONSTITUTE
OR IMPLY A RECOMMENDATION OR ENDORSEMENT BY PACIFIC BIOSCIENCES.
