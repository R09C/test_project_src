#!/bin/sh -e
# scripts/test.sh

BASEDIR=$(realpath "$(dirname "$0")")
ROOTDIR=$(realpath "$BASEDIR/..")
BINDIR="$ROOTDIR/build"  # Абсолютный путь к каталогу сборки
TESTDIR="$ROOTDIR/test"

echo "Running tests for comparer..."
python3 "$TESTDIR/comparer/checker.py" "$BINDIR/comparer" "$TESTDIR/comparer/test_data"

echo "Running tests for converter..."
python3 "$TESTDIR/converter/checker.py" "$BINDIR/converter" "$TESTDIR/converter/test_data" "$BINDIR/comparer"

echo "All tests completed."
