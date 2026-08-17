#!/bin/bash
# Basic smoke tests for codexion. Not graded, just here to sanity-check
# behaviour after making changes. Run from the repository root:
#   make && ./test/run_tests.sh

BIN=./codexion
PASS=0
FAIL=0

check() {
	if [ "$1" = "0" ]; then
		echo "PASS: $2"
		PASS=$((PASS + 1))
	else
		echo "FAIL: $2"
		FAIL=$((FAIL + 1))
	fi
}

echo "== argument validation =="
# these must all fail (non-zero exit), so we invert the check before passing
# it to check(), which treats 0 as success
$BIN >/dev/null 2>&1
[ $? -ne 0 ]; check $? "rejects missing arguments"

$BIN 5 100 100 100 100 5 100 bogus >/dev/null 2>&1
[ $? -ne 0 ]; check $? "rejects an unknown scheduler"

$BIN -3 100 100 100 100 5 100 fifo >/dev/null 2>&1
[ $? -ne 0 ]; check $? "rejects a negative number_of_coders"

echo
echo "== simulation completes without burnout when timing is generous =="
OUT=$($BIN 5 3000 200 200 200 4 50 fifo)
STOPPED=$?
BURNED=$(echo "$OUT" | grep -c "burned out")
COMPILES=$(echo "$OUT" | grep -c "is compiling")
[ "$STOPPED" -eq 0 ] && [ "$BURNED" -eq 0 ] && [ "$COMPILES" -eq 20 ]
check $? "5 coders each compile exactly 4 times, nobody burns out"

echo
echo "== a lone coder can never gather two dongles, so it burns out =="
OUT=$($BIN 1 500 200 200 200 3 50 fifo)
echo "$OUT" | grep -q "1 burned out"
check $? "single coder burns out"

echo
echo "== edf scheduling also completes cleanly =="
OUT=$($BIN 4 3000 200 200 200 3 50 edf)
BURNED=$(echo "$OUT" | grep -c "burned out")
COMPILES=$(echo "$OUT" | grep -c "is compiling")
[ "$BURNED" -eq 0 ] && [ "$COMPILES" -eq 12 ]
check $? "4 coders each compile exactly 3 times under edf"

echo
echo "$PASS passed, $FAIL failed"
[ "$FAIL" -eq 0 ]
