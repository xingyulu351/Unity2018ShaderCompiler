#!/bin/sh

# bash sets EUID; with dash, you need `id -u`
if test "x${EUID}" = 'x'; then
    EFFECTIVE_UID=`id -u`
else
    EFFECTIVE_UID=${EUID}
fi

EXTRACT_DIRECTORY=.
USAGE="Usage: $0 [-o|--output outputDirectory] [-h|--help]"


ARGS=`getopt -n "$0" -o o:h --long output:,help -- "$@"`
if test $? -ne 0; then
    # Bad options
    echo ${USAGE}
    exit 1
fi

# Evaluate arguments
eval set -- "$ARGS"
while true; do
    case "$1" in
        -o|--output)
            EXTRACT_DIRECTORY="$2"
            shift 2
        ;;
        -h|--help)
            echo ${USAGE}
            exit 0
        ;;
        --)
            # End of options
            shift
            break
        ;;
    esac
done

if test "x${EFFECTIVE_UID}" != "x0"; then
    echo "This installer must be run as root."
    exit 1
fi

EXTRACT_SUBDIR="${EXTRACT_DIRECTORY}/unity-editor-0.0.0f0"
echo "Installer for Unity 0.0.0f0"
echo ""
echo "Press Enter to begin extracting to ${EXTRACT_SUBDIR}"
read meh

# Get byte offset of embedded archive
ARCHIVE=`awk '/^__ARCHIVE_BEGINS_HERE__/ {print NR + 1; exit 0; }' $0`

# Actual extraction
echo "Unpacking Unity 0.0.0f0 ..."
mkdir -p "${EXTRACT_DIRECTORY}"
tail -n+${ARCHIVE} $0 | tar xj -C "${EXTRACT_DIRECTORY}"

# chrome-sandbox requires this: https://code.google.com/p/chromium/wiki/LinuxSUIDSandbox
chown root "${EXTRACT_SUBDIR}/Editor/chrome-sandbox"
chmod 4755 "${EXTRACT_SUBDIR}/Editor/chrome-sandbox"

echo "Extraction complete. Run ${EXTRACT_SUBDIR}/Editor/Unity to begin"

exit
__ARCHIVE_BEGINS_HERE__
