# pbcopper - CHANGELOG

## Active

### TODO
- Initial sequence-related data structures
- FireAndForget increase buffer

## [0.3.0] - 2018-04-30

### Changed
- Minor ToolContract::Config API change.

## [0.2.0] - 2017-12-03

### Added
- WorkQueue for parallel execution

## [0.1.1] - 2017-09-26

### Changed
- UNUSED macro to no-op method

## [0.1.0] - 2017-01-13

### Added
- Encapsulate project version and git sha1
- RSMovieName & RSReadName: Sequel and RSII movie names are composed of
different data, and thus need to be handled separately. MovieName & ReadName
handle Sequel-style data as the default.
- GoogleTest/GoogleMock in third-party/googletest
- Access to input command line from PacBio::CLI::Results
- QGramIndex & seed results
- Added support for new tool contract data model: option choices & modified JSON
format.
- Added basic option groups to CLI::Interface.

## [0.0.1] - 2016-06-22

### Added
- **PacBio::CLI** : command-line parsing & tool-contract integration
  - CLI - entry-point methods for parsing args & invoking application callback
  - Interface - define application's options, description, etc.
  - Results - contains the results of arg parsing (or resolved tool contract),
  will be passed to the application callback
  - ToolContract sub-namespace: contains all APIs for setting up tool contract
  integration
- **PacBio::Data**: main data types & data structures
  - Interval, Position, Zmw - basic data types (same as pbbam)
  - MovieName - query parts of a PacBio movie name
  - ReadName - query parts of a PacBio read name
- **PacBio::JSON** : JSON support
  - Json - wrapper around nlohmann::json
- **PacBio::Logging** : logging utilities
  - Logger, LogMessage, etc - basic logging (same as pbccs)
- **PacBio::Stream** : functional-programming-like data streaming
  - data Source, Sink, & Transform typedefs
  - instantiated with client-defined 'callables' (lambdas, free functions,
  static public member functions)
  - connected via operator >>
- **PacBio::Utility** : miscellaneous utilities
  - CallbackTimer - schedule periodic callback invokation (or delayed
  single-shot)
  - EnumClassHash - allows enums/enum classes to be used as STL hash keys
  - Stopwatch - timer for benchmarking, logging, etc
  - StringUtils - string dicing & splicing utilities
  - SystemInfo - currently provides endianness
- Examples
  - cli_demo - playground for testing the command line interface
- Tests
  - Unit tests for all classes/modules added

