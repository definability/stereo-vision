# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog]
and this project adheres to [Semantic Versioning].

## [Unreleased]

### Added

- `DisparityFinder` base class.

## 0.0.1 - 2017-03-16

### Added

- Basic project structure.
- `Matrix` class to represent images.
- `DisparityGraph` class to store images
  and calculate penalties for different labelings.
- `DisparityNode` to request and identify nodes of the graph.
- `Labeling` to represent, change and calculate penalty
  of particular labelings.
- `BFDisparityFinder` to find disparities using brute-force.

[Unreleased]: https://github.com/char-lie/stereo-vision/compare/v0.0.1...HEAD

[Keep a Changelog]: http://keepachangelog.com/en/1.0.0/
[Semantic Versioning]: http://semver.org/spec/v2.0.0.html
