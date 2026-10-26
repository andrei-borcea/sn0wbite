# Shared ASCII / color helpers for the sn0shell build+deploy scripts.
# source this file: `. "$(dirname "$0")/_ascii.sh"`

# -- colors (disabled when NO_COLOR is set or stdout is not a TTY) -------------
if [[ -t 1 && -z "${NO_COLOR:-}" ]]; then
    C_RST=$'\033[0m'
    C_DIM=$'\033[2m'
    C_BOLD=$'\033[1m'
    C_CYAN=$'\033[38;5;51m'
    C_BLUE=$'\033[38;5;39m'
    C_GREEN=$'\033[38;5;46m'
    C_YELLOW=$'\033[38;5;226m'
    C_MAGENTA=$'\033[38;5;201m'
    C_RED=$'\033[38;5;196m'
    C_SNOW=$'\033[38;5;159m'
    C_GREY=$'\033[38;5;240m'
else
    C_RST= C_DIM= C_BOLD= C_CYAN= C_BLUE= C_GREEN= C_YELLOW= C_MAGENTA= C_RED= C_SNOW= C_GREY=
fi

# -- banner --------------------------------------------------------------------
banner() {
    local tag="${1:-sn0wbite}"
    cat <<EOF
${C_SNOW}
        ███████╗███╗   ██╗ ██████╗ ██╗    ██╗██████╗ ██╗████████╗███████╗
        ██╔════╝████╗  ██║██╔═████╗██║    ██║██╔══██╗██║╚══██╔══╝██╔════╝
        ███████╗██╔██╗ ██║██║██╔██║██║ █╗ ██║██████╔╝██║   ██║   █████╗
        ╚════██║██║╚██╗██║████╔╝██║██║███╗██║██╔══██╗██║   ██║   ██╔══╝
        ███████║██║ ╚████║╚██████╔╝╚███╔███╔╝██████╔╝██║   ██║   ███████╗
        ╚══════╝╚═╝  ╚═══╝ ╚═════╝  ╚══╝╚══╝ ╚═════╝ ╚═╝   ╚═╝   ╚══════╝${C_RST}
${C_DIM}             ·  Watch7,11 · watchOS 27 · QEMU-SPTM userspace  ·${C_RST}
${C_DIM}             ·                [ ${C_SNOW}${tag}${C_RST}${C_DIM} ]                ·${C_RST}
EOF
}

# -- step markers --------------------------------------------------------------
step() {
    local n="$1" total="$2"; shift 2
    printf '\n%s[%s/%s]%s %s%s%s\n' \
        "${C_BLUE}${C_BOLD}" "${n}" "${total}" "${C_RST}" \
        "${C_CYAN}" "$*" "${C_RST}"
}

info()    { printf '  %s•%s %s\n' "${C_DIM}" "${C_RST}" "$*"; }
ok()      { printf '  %s✓%s %s%s%s\n' "${C_GREEN}" "${C_RST}" "${C_GREEN}" "$*" "${C_RST}"; }
warn()    { printf '  %s!%s %s%s%s\n' "${C_YELLOW}" "${C_RST}" "${C_YELLOW}" "$*" "${C_RST}"; }
die()     { printf '  %s✗%s %s%s%s\n' "${C_RED}"   "${C_RST}" "${C_RED}"   "$*" "${C_RST}" >&2; exit 1; }
kv()      { printf '    %s%-14s%s %s\n' "${C_GREY}" "$1" "${C_RST}" "$2"; }

divider() {
    printf '%s' "${C_DIM}"
    printf '─%.0s' $(seq 1 72)
    printf '%s\n' "${C_RST}"
}

# -- fancy final box -----------------------------------------------------------
# usage: finalbox "TITLE" "key1=value1" "key2=value2" ...
finalbox() {
    local title="$1"; shift
    local line
    printf '\n%s╔' "${C_GREEN}"
    printf '═%.0s' $(seq 1 72)
    printf '╗%s\n' "${C_RST}"
    printf '%s║%s %s%s%-70s%s%s║%s\n' "${C_GREEN}" "${C_RST}" "${C_BOLD}${C_GREEN}" "" "${title}" "${C_RST}" "${C_GREEN}" "${C_RST}"
    printf '%s╠' "${C_GREEN}"; printf '═%.0s' $(seq 1 72); printf '╣%s\n' "${C_RST}"
    for line in "$@"; do
        local key="${line%%=*}" val="${line#*=}"
        printf '%s║%s %s%-14s%s %-55s %s║%s\n' \
            "${C_GREEN}" "${C_RST}" "${C_CYAN}" "${key}" "${C_RST}" "${val}" "${C_GREEN}" "${C_RST}"
    done
    printf '%s╚' "${C_GREEN}"
    printf '═%.0s' $(seq 1 72)
    printf '╝%s\n' "${C_RST}"
}
