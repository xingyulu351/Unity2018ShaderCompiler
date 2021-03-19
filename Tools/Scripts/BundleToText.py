import os
import sys
import subprocess
import shutil
import argparse

parser = argparse.ArgumentParser(description = "Extract the content of an asset bundle into a series of text files. The text files will be place in the same folder as the bundle. WebExtract.exe and Binary2Text.exe will be built if necessary")
parser.add_argument('BundlePath', type=str, nargs='+',help = "One or more files or directories that are, or contain, bundles")
parser.add_argument('-r', dest="recursive", action='store_const', const=True, default=False, help="Recursively search directories")
parser.add_argument('-v', dest="verbose", action='store_const', const=True, default=False, help="Verbose ouput")
parser.add_argument('-b', dest="fullbinaryblob", action='store_const', const=True, default=False, help="Include large binary blobs instead of just their hash" )
parser.add_argument('-hexfloat', dest="hexfloat", action='store_const', const=True, default=False, help="Show hex value alongside floats" )

gArgs = parser.parse_args()

scriptPath = os.path.dirname(os.path.realpath(__file__))
repoPath = os.path.abspath(os.path.join(scriptPath,"../.."))

#TODO: mac this work on mac
toolsPath = os.path.join(repoPath, "build/WindowsEditor/Data/Tools/")
webExtractPath = os.path.join(toolsPath, "WebExtract.exe")
binaryToTextPath = os.path.join(toolsPath, "Binary2Text.exe")
jamPath = os.path.join(repoPath, "jam.bat")

def RunProcess(args, cwd=os.getcwd()):

    pipe = None if gArgs.verbose else subprocess.PIPE

    if gArgs.verbose:
        print ("%s>%s" % (cwd,' '.join(args)))
    p = subprocess.Popen( args, cwd=cwd, stdout = pipe)
    (out, err) = p.communicate()

    if p.returncode != 0:
        raise Exception("FAILED. Error Code %d\n%s" % (p.returncode, out + err))

    return p.returncode

def PrintNoNewline(text):
    sys.stdout.write(text)
    sys.stdout.flush()

def BuildDependencies():

    if not os.path.isfile(binaryToTextPath):
        PrintNoNewline("%s did not exist. building ... " % (binaryToTextPath))
        RunProcess([jamPath, "Binary2Text"], repoPath)
        assert(os.path.isfile(binaryToTextPath))
        print("Complete")

    if not os.path.isfile(webExtractPath):
        PrintNoNewline("%s did not exist. building ... " % (webExtractPath))
        RunProcess([jamPath, "WebExtract"], repoPath)
        assert(os.path.isfile(webExtractPath))
        print("Complete")

def ProcessBundle(path):

    dataPath = "%s_data" % path
    shutil.rmtree(dataPath, ignore_errors=True)
    RunProcess([webExtractPath, path], toolsPath)

    dirname = os.path.dirname(path)
    dataPath = "%s_data" % path
    binaryFiles = [os.path.join(dataPath,x) for x in next(os.walk(dataPath))[2] if not x.lower().endswith(".ress")]
    binary2TextFlags = [] if gArgs.fullbinaryblob else ["-largebinaryhashonly"]
    binary2TextFlags = binary2TextFlags + (["-hexfloat"] if gArgs.hexfloat else [])

    for binaryFile in binaryFiles:
        RunProcess([binaryToTextPath, binaryFile] + binary2TextFlags, toolsPath)

    for filename in next(os.walk(dataPath))[2]:
        if filename.endswith(".txt"):
            srcFilename = os.path.join(dataPath, filename)
            dstFilename = os.path.join(dirname, "%s_%s" % (os.path.basename(path), filename))
            shutil.move(srcFilename, dstFilename)

    shutil.rmtree(dataPath, ignore_errors=True)

def GatherBundleNames(initialPaths, recursive):
    toProcess = list(initialPaths)
    ret = []
    while len(toProcess) > 0:
        path = toProcess.pop()
        if os.path.isdir(path):
            if (recursive or (path in initialPaths)):
                toProcess.extend( os.path.join(path, name) for name in os.listdir(path) )
        else:
            if len(os.path.splitext(path)[1]) == 0:
                ret.append(path)
    return ret

BuildDependencies()

toExtract = GatherBundleNames([os.path.abspath(x) for x in gArgs.BundlePath], gArgs.recursive)

print("Extracting %d bundles:" % len(toExtract))
for path in toExtract:
    PrintNoNewline("Processing %s ... " % path)
    try:
        ProcessBundle(path)
        print("Complete")
    except Exception as e:
        print("Failed")
        print(e)
