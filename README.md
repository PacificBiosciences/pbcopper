# pbcopper [![CircleCI](https://circleci.com/gh/PacificBiosciences/pbcopper.svg?style=svg&circle-token=4fe4468113a44b8b76019c901f1b6930ef26944b)](https://circleci.com/gh/PacificBiosciences/pbcopper)

The **pbcopper** library provides a suite of data structures, algorithms, and utilities 
for C++ applications. 

## Getting Started

Download repo & compile library:

```sh
git clone https://github.com/PacificBiosciences/pbcopper.git && cd pbcopper
mkdir build && cd build
cmake ..
make
```
 
To build & run tests:

```sh
make check
```

To build examples:

```sh
make examples
```

To build Doxygen API documentation: (**not yet available**)

```sh
make docs
```

## API Overview - main modules 

- **PacBio::CLI** : command-line parsing & tool-contract integration
  - CLI - entry-point methods for parsing args & invoking application callback
  - Interface - define application's options, description, etc.
  - Results - contains the results of arg parsing (or resolved tool contract), will be passed to the application callback
  - ToolContract sub-namespace: contains all APIs for setting up tool contract integration
- **PacBio::Data**: main data types & data structures
  - Interval, Position, Zmw - basic data types (same as pbbam)
  - MovieName - query parts of a PacBio movie name
  - ReadName - query parts of a PacBio read name
- **PacBio::JSON** : JSON support
  - Json - wrapper around [nlohmann::json](https://github.com/nlohmann/json) 
- **PacBio::Logging** : logging utilities
  - Logger, LogMessage, etc - basic logging (same as pbccs)
- **PacBio::Stream** : functional-programming-like data streaming
  - data Source, Sink, & Transform typedefs
  - instantiated with client-defined 'callables' (lambdas, free functions, static public member functions)
  - connected via operator >> 
- **PacBio::Utility** : miscellaneous utilities
  - CallbackTimer - schedule periodic callback invokation (or delayed single-shot)
  - EnumClassHash - allows enums/enum classes to be used as STL hash keys
  - Stopwatch - timer for benchmarking, logging, etc
  - StringUtils - string dicing & splicing utilities
  - SystemInfo - currently provides endianness 

##  Documentation

  - Main docs coming soon.
  - [Changelog](https://github.com/PacificBiosciences/pbcopper/blob/master/CHANGELOG.md)

## License

 - [PacBio open source license](https://github.com/PacificBiosciences/pbcopper/blob/master/LICENSE.txt)
