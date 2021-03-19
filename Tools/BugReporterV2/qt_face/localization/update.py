#! /usr/bin/python

# Helper script
# Shortcut for running lupdate with correct arguments

from subprocess import call

# Please specify your path
lupdate = "c:/Qt/5.3/msvc2012_opengl/bin/lupdate.exe"

call([lupdate, "-verbose", "-pro", "./files"])
