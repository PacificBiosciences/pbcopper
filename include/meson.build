###########
# headers #
###########

if not meson.is_subproject()

  # pbcopper
  install_headers(
    files([
      'pbcopper/LibraryInfo.h',
      'pbcopper/PbcopperConfig.h']),
    subdir : 'pbcopper')

  # pbcopper/algorithm
  install_headers(
    files([
      'pbcopper/algorithm/Heteroduplex.h',
      'pbcopper/algorithm/KMerIndex.h',
      'pbcopper/algorithm/LSHIndex.h',
    ]),
    subdir : 'pbcopper/algorithm')

  # pbcopper/algorithm/internal
  install_headers(
    files([
      'pbcopper/algorithm/internal/HeteroduplexUtils.h',
    ]),
    subdir : 'pbcopper/algorithm/internal')

  # pbcopper/align
  install_headers(
    files([
      'pbcopper/align/AffineAlignment.h',
      'pbcopper/align/AlignConfig.h',
      'pbcopper/align/BandedChainAlignment.h',
      'pbcopper/align/ChainSeeds.h',
      'pbcopper/align/ChainSeedsConfig.h',
      'pbcopper/align/EdlibAlign.h',
      'pbcopper/align/FindSeeds.h',
      'pbcopper/align/GlobalLocalAlignment.h',
      'pbcopper/align/LinearAlignment.h',
      'pbcopper/align/LocalAlignment.h',
      'pbcopper/align/PairwiseAlignment.h',
      'pbcopper/align/Seed.h',
      'pbcopper/align/Seeds.h',
      'pbcopper/align/SparseAlignment.h']),
    subdir : 'pbcopper/align')

  # pbcopper/align/internal
  install_headers(
    files([
      'pbcopper/align/internal/BCAlignBlocks.h',
      'pbcopper/align/internal/BCAlignImpl.h']),
    subdir : 'pbcopper/align/internal')

   # pbcopper/align/cssw
  install_headers(
    files([
      'pbcopper/align/cssw/ssw.h',
      'pbcopper/align/cssw/ssw_cpp.h']),
    subdir : 'pbcopper/align/cssw')

  # pbcopper/cli2
  install_headers(
    files([
      'pbcopper/cli2/CLI.h',
      'pbcopper/cli2/Interface.h',
      'pbcopper/cli2/MultiToolInterface.h',
      'pbcopper/cli2/Option.h',
      'pbcopper/cli2/OptionValue.h',
      'pbcopper/cli2/OptionGroup.h',
      'pbcopper/cli2/PositionalArgument.h',
      'pbcopper/cli2/Result.h',
      'pbcopper/cli2/Results.h',
      'pbcopper/cli2/ResultsHandler.h',
      'pbcopper/cli2/Tool.h',
      'pbcopper/cli2/VersionPrinterCallback.h',
      ]),
    subdir : 'pbcopper/cli2')

  # pbcopper/cli2/internal
  install_headers(
    files([
      'pbcopper/cli2/internal/BuiltinOptions.h',
      'pbcopper/cli2/internal/CommandLineParser.h',
      'pbcopper/cli2/internal/HelpMetrics.h',
      'pbcopper/cli2/internal/HiddenOptionMode.h',
      'pbcopper/cli2/internal/InterfaceData.h',
      'pbcopper/cli2/internal/InterfaceHelpPrinter.h',
      'pbcopper/cli2/internal/MultiToolInterfaceData.h',
      'pbcopper/cli2/internal/MultiToolInterfaceHelpPrinter.h',
      'pbcopper/cli2/internal/OptionData.h',
      'pbcopper/cli2/internal/OptionTranslator.h',
      'pbcopper/cli2/internal/PositionalArgumentData.h',
      'pbcopper/cli2/internal/PositionalArgumentTranslator.h',
      'pbcopper/cli2/internal/VersionPrinter.h',]),
    subdir : 'pbcopper/cli2/internal')

  # pbcopper/container
  install_headers(
    files([
      'pbcopper/container/BitContainer.h',
      'pbcopper/container/BitConversion.h',
      'pbcopper/container/BitmaskContainer.h',
      'pbcopper/container/CapacityPointer.h',
      'pbcopper/container/DNAString.h',
      'pbcopper/container/RHUnordered.h',
      'pbcopper/container/Unordered.h',
    ]),
    subdir : 'pbcopper/container')

  # pbcopper/cuda
  install_headers(
    files([
      'pbcopper/cuda/Cuda.h',
    ]),
    subdir : 'pbcopper/cuda')

  # pbcopper/dagcon
  install_headers(
    files([
      'pbcopper/dagcon/Alignment.h',
      'pbcopper/dagcon/AlignmentGraph.h',
    ]),
    subdir : 'pbcopper/dagcon')

  # pbcopper/data
  install_headers(
    files([
      'pbcopper/data/Accuracy.h',
      'pbcopper/data/CCSTag.h',
      'pbcopper/data/Cigar.h',
      'pbcopper/data/CigarOperation.h',
      'pbcopper/data/Clipping.h',
      'pbcopper/data/FrameCodec.h',
      'pbcopper/data/FrameEncoders.h',
      'pbcopper/data/Frames.h',
      'pbcopper/data/GenomicInterval.h',
      'pbcopper/data/Interval.h',
      'pbcopper/data/IntervalTree.h',
      'pbcopper/data/LocalContextFlags.h',
      'pbcopper/data/MappedRead.h',
      'pbcopper/data/MovieName.h',
      'pbcopper/data/Orientation.h',
      'pbcopper/data/Position.h',
      'pbcopper/data/QualityValue.h',
      'pbcopper/data/QualityValues.h',
      'pbcopper/data/Read.h',
      'pbcopper/data/ReadId.h',
      'pbcopper/data/ReadName.h',
      'pbcopper/data/RSMovieName.h',
      'pbcopper/data/RSReadName.h',
      'pbcopper/data/SNR.h',
      'pbcopper/data/Strand.h',
      'pbcopper/data/Zmw.h']),
    subdir : 'pbcopper/data')

  # pbcopper/data/internal
  install_headers(
    files([
      'pbcopper/data/internal/ClippingImpl.h',
      'pbcopper/data/internal/MovieName-inl.h',
      'pbcopper/data/internal/ReadNameBase-inl.h',
      'pbcopper/data/internal/ReadNameBase.h',
      'pbcopper/data/internal/RSMovieName-inl.h']),
    subdir : 'pbcopper/data/internal')

  # pbcopper/json
  install_headers(
    files([
      'pbcopper/json/JSON.h']),
    subdir : 'pbcopper/json')

  # pbcopper/json/internal
  install_headers(
    files([
      'pbcopper/json/internal/json.hpp']),
    subdir : 'pbcopper/json/internal')

  # pbcopper/library
  install_headers(
    files([
      'pbcopper/library/Bundle.h',
      'pbcopper/library/Info.h',
    ]),
    subdir : 'pbcopper/library')

  # pbcopper/logging
  install_headers(
    files([
      'pbcopper/logging/Logging.h',
      'pbcopper/logging/LogConfig.h',
      'pbcopper/logging/LogField.h',
      'pbcopper/logging/LogLevel.h',
      'pbcopper/logging/LogMessage.h']),
    subdir : 'pbcopper/logging')

  # pbcopper/math
  install_headers(
    files([
      'pbcopper/math/ConfusionMatrix.h',
      'pbcopper/math/FishersExact.h',
    ]),
    subdir : 'pbcopper/math')

  # pbcopper/parallel
  install_headers(
    files([
      'pbcopper/parallel/FireAndForget.h',
      'pbcopper/parallel/FireAndForgetIndexed.h',
      'pbcopper/parallel/ThreadCount.h',
      'pbcopper/parallel/WorkQueue.h']),
    subdir : 'pbcopper/parallel')

  # pbcopper/pbmer
  install_headers(
    files([
      'pbcopper/pbmer/Bubble.h',
      'pbcopper/pbmer/Dbg.h',
      'pbcopper/pbmer/DbgNode.h',
      'pbcopper/pbmer/DnaBit.h',
      'pbcopper/pbmer/Kmer.h',
      'pbcopper/pbmer/Mers.h',
      'pbcopper/pbmer/Parser.h',
      'pbcopper/pbmer/SparseHaplotype.h',
      'pbcopper/pbmer/KFGraph.h',
      'pbcopper/pbmer/KFNode.h',
      ]),
    subdir : 'pbcopper/pbmer')

  # pbcopper/poa
  install_headers(
    files([
      'pbcopper/poa/PoaConsensus.h',
      'pbcopper/poa/PoaGraph.h',
      'pbcopper/poa/RangeFinder.h',
      'pbcopper/poa/SparsePoa.h',
    ]),
    subdir : 'pbcopper/poa')

  # pbcopper/reports
  install_headers(
    files([
      'pbcopper/reports/Report.h']),
    subdir : 'pbcopper/reports')

  # pbcopper/qgram
  install_headers(
    files([
      'pbcopper/qgram/Index.h',
      'pbcopper/qgram/IndexHit.h',
      'pbcopper/qgram/IndexHits.h']),
    subdir : 'pbcopper/qgram')

  # pbcopper/qgram/internal
  install_headers(
    files([
      'pbcopper/qgram/internal/Hashing-inl.h',
      'pbcopper/qgram/internal/Index-inl.h']),
    subdir : 'pbcopper/qgram/internal')

  # pbcopper/third-party
  install_headers(
    files([
      'pbcopper/third-party/edlib.h']),
    subdir : 'pbcopper/third-party')

  # pbcopper/third-party/htslib
  install_headers(
    files([
      'pbcopper/third-party/htslib/kfunc.h']),
    subdir : 'pbcopper/third-party/htslib')

  # pbcopper/third-party/kxsort
  install_headers(
    files([
      'pbcopper/third-party/kxsort/kxsort.h']),
    subdir : 'pbcopper/third-party/kxsort')

  # pbcopper/third-party/ksw2
  install_headers(
    files([
      'pbcopper/third-party/ksw2/kalloc.h',
      'pbcopper/third-party/ksw2/ksw2.h',
    ]),
    subdir : 'pbcopper/third-party/ksw2')

  # pbcopper/third-party/rigtorp
  install_headers(
    files([
      'pbcopper/third-party/rigtorp/MPMCQueue.hpp',
      'pbcopper/third-party/rigtorp/SPSCQueue.hpp',
      ]),
    subdir : 'pbcopper/third-party/rigtorp')

  # pbcopper/third-party/robin_hood
  install_headers(
    files([
      'pbcopper/third-party/robin_hood/robin_hood.h']),
    subdir : 'pbcopper/third-party/robin_hood')

  # pbcopper/third-party/sparc
  install_headers(
    files([
      'pbcopper/third-party/sparc/sparc.hpp']),
    subdir : 'pbcopper/third-party/sparc')

  # pbcopper/utility
  install_headers(
    files([
      'pbcopper/utility/Alarm.h',
      'pbcopper/utility/Base64.h',
      'pbcopper/utility/Deleters.h',
      'pbcopper/utility/FastMod.h',
      'pbcopper/utility/FileUtils.h',
      'pbcopper/utility/Intrinsics.h',
      'pbcopper/utility/MemoryConsumption.h',
      'pbcopper/utility/MinMax.h',
      'pbcopper/utility/MoveAppend.h',
      'pbcopper/utility/MoveInsert.h',
      'pbcopper/utility/OStreamRedirect.h',
      'pbcopper/utility/PartitionElements.h',
      'pbcopper/utility/PbcopperVersion.h',
      'pbcopper/utility/Random.h',
      'pbcopper/utility/SafeSubtract.h',
      'pbcopper/utility/SequenceUtils.h',
      'pbcopper/utility/Shuffle.h',
      'pbcopper/utility/Ssize.h',
      'pbcopper/utility/Stopwatch.h',
      'pbcopper/utility/StringUtils.h',
      'pbcopper/utility/TimeReporter.h',
      'pbcopper/utility/Unused.h']),
    subdir : 'pbcopper/utility')

  # pbcopper/utility/internal
  install_headers(
    files([
      'pbcopper/utility/internal/FileUtils-inl.h',
      'pbcopper/utility/internal/Stopwatch-inl.h',
    ]),
    subdir : 'pbcopper/utility/internal')
endif

pbcopper_include_directories = include_directories('.')
