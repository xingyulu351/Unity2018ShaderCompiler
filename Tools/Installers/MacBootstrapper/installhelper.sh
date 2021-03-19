#!/bin/sh

LOG=/var/log/unityinstaller.log

log () {
    d=`date`
    echo "[$d] $@" >>$LOG
}

Exit () {
    log $1
    echo "EXIT:$2"
    exit $2
}

# caller sends commands and their arguments 1 thing per line
#
# supported commands:
#
# installpkg FULL_PACKAGE_PATH TARGET_VOLUME_PATH
#  tells to install the package
#
# example:
#    installpkg
#    /Users/username/Downloads/Unity.pkg
#    /Volumes/Macintosh HD
#
# installdmg FULL_DMG_PATH
#  Installs single .app from .dmg into /Applications
#
# example:
#    installdmg
#    /Users/username/Downloads/Tool.dmg
#
# quit
#  tells to quit
#

# This script will automatically quit on stdin EOF

log "Started Unity installation helper"

while true; do
    read input || Exit "End of command stream, exiting." 0 # end of command stream
    log "input: $input"
    case $input in
    installpkg)
        read package || Exit "Incomplete install command, couldn't read package path" 2 # incomplete install command
        log "package: $package"

        read target || Exit "Incomplete install command, couldn't read target volume path" 2 # incomplete install command
        log "target: $target"
        log "cmd: /usr/sbin/installer -pkg \"$package\" -target \"$target\" -verboseR"

        # invoke installer with received arguments
        /usr/sbin/installer -pkg "$package" -target "$target" -verboseR
        err=$?

        log "The package installed successfully"
        echo "EXIT:$err" # send back the exit code from /usr/bin/installer
    ;;
    installdmg)
        read dmg || Exit "Incomplete install command, couldn't read disk image path" 2 # incomplete install command
        log "dmg: $dmg"

        TMPDIR=/tmp/unity-dmg-$RANDOM
        log "Temp Mount Dir: $TMPDIR"

        echo "installer:%0"
        echo "installer:PHASE:Verifying and mounting disk image..."
        log "cmd: hdiutil attach \"$dmg\" -mountpoint \"$TMPDIR\" -nobrowse -puppetstrings"
        hdiutil attach "$dmg" -mountpoint "$TMPDIR" -nobrowse -puppetstrings || Exit "Could not mount $dmg at $TMPDIR" 3

        log "find \"$TMPDIR\" -name '*.app' -type d -mindepth 1 -maxdepth 1"
        find "$TMPDIR" -name '*.app' -type d -mindepth 1 -maxdepth 1 || Exit "Could not find .app in mounted .dmg" 4

        APP_PATH=`find "$TMPDIR" -name '*.app' -type d -mindepth 1 -maxdepth 1`
        APP_FILENAME=$(basename "$APP_PATH")
        DEST_PATH="/Applications/"

        log "App Path: $APP_PATH"
        log "App Filename: $APP_FILENAME"
        log "Dest Path: $DEST_PATH"

        echo "installer:PHASE:Installing application..."
        log "rsync -r --progress \"$APP_PATH\" \"$DEST_PATH\""
        rsync -r --progress "$APP_PATH" "$DEST_PATH" || Exit "Copy $APP_PATH to $DEST_PATH failed" 5

        log "/usr/bin/touch \"$DEST_PATH\""
        touch "$DEST_PATH" || Exit "Touch $DEST_PATH failed" 6

        log "/usr/bin/hdiutil detach \"$TMPDIR\""
        hdiutil detach "$TMPDIR" # Don't care about failed detach

        echo "installer:PHASE:$APP_PATH was successfully installed"
        log "The disk image installed successfully"
        echo "EXIT:0"
    ;;
    quit)
        Exit "The helper finished with no errors" 0 # caller told us to quit
    ;;
    esac
done
