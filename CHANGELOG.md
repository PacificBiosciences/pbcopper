# pbcopper - CHANGELOG

## Active

### Added
 - Base64 encoding/decoding
 - (Simplified) alarm file can be created for general exceptions
 - Utility::StringView as std::string_view look-a-like

### Changed
 - Accuracy explicitly allows -1, otherwise clamps to [0.0, 1.0]

## [1.7.0] - 2020-05-22

### Added
 - PartitionElements utility
 - Version printing callbacks for CLI
 - Ssize utility
 - Knock-first graph
 - VERBOSE log level
 - Dagcon API
 - LibraryInfo API

### Changed
 - Now using (signed) Data::Position in Data::Read.
 - Data::SNR values changed from double to float.

### Fixed
 - ReadId from CCS read names
 - More robust random number generators (for UUIDs).
   * getrandom() requires Linux kernel 3.17+
   * getentropy() requires glibc 2.25+

### Removed
 - Unused SystemInfo utility

## [1.6.0] - 2020-04-10

### Removed
 - CLI version 1 (pbcopper/cli/*)

## [1.5.0] - 2020-03-12

### Added
 - pbmer API - DeBruijn graph & kmers
 - AlarmException for generating JSON alarms
 - Allow SI suffixes in command line arguments
 - std::ostream redirection utilities

### Changed
 - Double-precision in Stopwatch

### Fixed
 - WorkQueue deadlock if exception was thrown in consumer task

### Removed
 - InstallSignalHandlers() from logging

## [1.4.0] - 2019-11-05

### Added
 - Consolidated & cleanup of core data structures shared between pbcopper/unanimity/pbbam
 - CLI_v2 now maintains logger setup & lifetime
 - Alarms API

## [1.3.0] - 2019-06-11

### Added
 - Alignment algorithms from unanimity (local alignment, semi-global, affine, etc)

### Removed
 - Unused CallbackTimer
 - Unused PacBio::Stream utilities

## [1.2.0] - 2019-05-03

### Changed
 - CLI_v2::Results::NumProcessors() changed to CLI_v2::Results::NumThreads() for clarity.

### Added
 - Convenience methods for CLI_v2 clients: default log level override, explicit access to built-ins, built-in 'num-threads' option.

## [1.1.0] - 2019-04-29

### Added
 - Reports module

## [1.0.0] - 2019-04-23

### Changed
 - C++14 is now a *hard* minimum.

### Removed
 - Headers emulating C++14 features for C++11.

### Added
 - New (opt-in) CLI version 2.

## [0.5.0] - 2019-04-11

### Changed
 - Requires C++14 at minimum.

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

