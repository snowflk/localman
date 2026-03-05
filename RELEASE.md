# Release process

This document describes the manual release workflow for `localman`.

## Versioning

`localman` follows semantic versioning: `MAJOR.MINOR.PATCH`.

- **MAJOR**: incompatible changes
- **MINOR**: new features, backwards compatible
- **PATCH**: bug fixes, backwards compatible

The release version lives in `VERSION`. The `Makefile` reads it to name archives.

## Checklist

### 1) Update version
- Set the new version in `VERSION` (for this release: `1.2.0`).
### 2) Build and package
- Build the binary:
  - `make clean && make`
- Create a distributable archive:
  - `make dist`
- Verify the archive contains expected files:
  - `tar -tzf localman-1.2.0.tar.gz`

### 4) Verify behavior
- Run a quick smoke test:
  - `./localman ls`
  - `./localman add example.local`
  - `./localman ls`
  - `./localman rm example.local`
  - `./localman clear`

### 5) Tag and release
- Commit changes:
  - `git add VERSION CHANGELOG.md`
  - `git commit -m "Release v1.2.0"`
- Tag the release:
  - `git tag -a v1.2.0 -m "v1.2.0"`
- Push:
  - `git push`
  - `git push --tags`

### 6) Publish release artifacts
- Create a GitHub Release for `v1.2.0`.
- Attach `localman-1.2.0.tar.gz`.

## Rollback
- If a release is bad:
  - Fix the issue in a new patch release (e.g., `1.2.1`).
  - Repeat this checklist.
