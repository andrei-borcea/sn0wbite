#!/bin/sh
# Force REA's rizin-backed commands (execute-rizin-command, debug-with-rizin,
# the decompile rizin path) to run radare2 instead of rizin.
#
# REA resolves the `rizin` executable from PATH (provider_executable: rizin).
# radare2 is a sibling of rizin and accepts the same invocation REA uses
# (`-N -q -c <cmd> <path>`) and the core commands (aa/aar/axt/af/pdf/pd/is/iz…),
# so a PATH shim named `rizin` that execs `r2` transparently redirects REA.
#
# Usage:  tools/.../rea-r2.sh execute-rizin-command <artifact> '<r2 commands>'
#         tools/.../rea-r2.sh decompile <artifact> <addr> ...
# Any rea-agents subcommand/args are forwarded verbatim.
#
# Env expected (kernelcache RE on this Mac): GHIDRA_INSTALL_DIR, JAVA_HOME are
# only needed for the Ghidra provider; the r2 path needs neither.
set -e
R2="$(command -v r2 || echo /opt/homebrew/bin/r2)"
SHIM="$(mktemp -d)"
cat > "$SHIM/rizin" <<SH
#!/bin/sh
exec "$R2" "\$@"
SH
chmod +x "$SHIM/rizin"
trap 'rm -rf "$SHIM"' EXIT
PATH="$SHIM:$PATH" exec npx -y rea-agents@6.7.0 "$@"
