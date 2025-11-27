#!/bin/sh
# Brainfuck interpreter test suite
set -e

BF="${1:-./bf}"
TESTS_DIR="$(dirname "$0")/tests"
PASS=0 FAIL=0

red() { printf "\033[31m%s\033[0m" "$1"; }
green() { printf "\033[32m%s\033[0m" "$1"; }

# Test helper: compare output
test_output() {
    name="$1" expected="$2"; shift 2
    actual=$("$BF" "$@" 2>&1) || true
    if [ "$actual" = "$expected" ]; then
        green "✓"; echo " $name"
        PASS=$((PASS + 1))
    else
        red "✗"; echo " $name"
        echo "  expected: $expected"
        echo "  got:      $actual"
        FAIL=$((FAIL + 1))
    fi
}

# Test helper: check exit code
test_exit() {
    name="$1" expected="$2"; shift 2
    "$BF" "$@" >/dev/null 2>&1 && actual=0 || actual=$?
    if [ "$actual" -eq "$expected" ]; then
        green "✓"; echo " $name"
        PASS=$((PASS + 1))
    else
        red "✗"; echo " $name (exit $actual, expected $expected)"
        FAIL=$((FAIL + 1))
    fi
}

# Test helper: run .bf file against .expected
test_file() {
    bf_file="$1"
    name=$(basename "$bf_file" .bf)
    expected_file="${bf_file%.bf}.expected"
    input_file="${bf_file%.bf}.input"

    if [ ! -f "$expected_file" ]; then
        echo "? $name (no .expected file)"
        return
    fi

    expected=$(cat "$expected_file")
    if [ -f "$input_file" ]; then
        actual=$("$BF" "$bf_file" < "$input_file" 2>&1) || true
    else
        actual=$("$BF" "$bf_file" 2>&1) || true
    fi

    if [ "$actual" = "$expected" ]; then
        green "✓"; echo " $name"
        PASS=$((PASS + 1))
    else
        red "✗"; echo " $name"
        printf "  expected: %s\n" "$(echo "$expected" | head -c 50)"
        printf "  got:      %s\n" "$(echo "$actual" | head -c 50)"
        FAIL=$((FAIL + 1))
    fi
}

echo "=== Unit Tests ==="
test_output "empty program" "" -c ""
test_output "single char" "A" -c "++++++++[>++++++++<-]>+."
test_output "cell wrap 255→0" "" -c "-[+]"
test_output "cell wrap 0→255" "" -c "+[-]"
test_output "pointer wrap" "" -c "<"

echo ""
echo "=== Error Handling ==="
test_exit "unmatched [" 1 -c "["
test_exit "unmatched ]" 1 -c "]"
test_exit "missing file" 1 nonexistent.bf
test_exit "no arguments" 1
test_exit "-c without code" 1 -c

echo ""
echo "=== CLI ==="
test_exit "help -h" 0 -h
test_exit "help --help" 0 --help
test_exit "valid program" 0 -c "+++."

echo ""
echo "=== File Tests ==="
for f in "$TESTS_DIR"/*.bf; do
    [ -f "$f" ] && test_file "$f"
done

echo ""
if [ "$FAIL" -eq 0 ]; then
    green "All $PASS tests passed"
    echo ""
else
    red "$FAIL failed"; echo ", $PASS passed"
fi
[ "$FAIL" -eq 0 ]
