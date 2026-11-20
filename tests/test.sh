#!/bin/sh
set -eu

bin=${1:-./launcher}
tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT

fail() {
    echo "FAIL: $*" >&2
    exit 1
}

# No arguments: print usage and exit nonzero.
if "$bin" >"$tmp/out" 2>"$tmp/err"; then
    fail "expected nonzero exit status with no arguments"
fi
grep -q "usage:" "$tmp/err" || fail "missing usage message"

# Launch a script and forward its arguments and exit status.
cat >"$tmp/hello" <<'EOF'
#!/bin/sh
printf 'hello %s\n' "$1"
exit 7
EOF
chmod +x "$tmp/hello"

set +e
"$bin" "$tmp/hello" world >"$tmp/out" 2>"$tmp/err"
status=$?
set -e

[ "$status" -eq 7 ] || fail "expected exit status 7, got $status"
[ "$(cat "$tmp/out")" = "hello world" ] || fail "arguments were not forwarded"

# A missing file: report the failure and exit nonzero.
if "$bin" "$tmp/does-not-exist" >"$tmp/out" 2>"$tmp/err"; then
    fail "expected nonzero exit status for a missing file"
fi
grep -q "does-not-exist" "$tmp/err" || fail "error message does not name the file"

echo "all tests passed"
