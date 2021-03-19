#! /usr/bin/python

# Helper script
# Shortcut for running lrelease for every file with correct arguments

import os
from subprocess import call

# Please specify your path
lrelease = "c:/Qt/5.3/msvc2012_opengl/bin/lrelease.exe"

ts_files = os.listdir("./ts")

for ts in ts_files:
    call([
        lrelease,
        os.path.join("ts", ts),
        "-qm",
        os.path.join("qm", os.path.splitext(ts)[0] + ".qm")])
