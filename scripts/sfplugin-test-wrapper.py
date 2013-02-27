#!/usr/bin/env python

import os
import sys
import subprocess
import shutil
import commands

def runCommand(workingDir, bextractLocation, sfpluginLocation, extraBextractCmdArgs,
               mfFileLocation, outputMplFile, testFile, goodFilename):
    os.chdir(workingDir)

    bextractCmd = ("%s %s -t %s -p %s") % (bextractLocation, extraBextractCmdArgs, mfFileLocation, outputMplFile)
    a = commands.getoutput(bextractCmd)

    sfpluginCmd = ("%s -pl %s %s") % (sfpluginLocation, outputMplFile, testFile)
    a = commands.getoutput(sfpluginCmd)

    returnCode = compareSfpluginOutput(a, goodFilename)
    return returnCode
    

def compareSfpluginOutput(oneData, twoFilename):
    oneLines = oneData.split("\n")
    twoLines = open(twoFilename).readlines()
    for a, b in zip(oneLines, twoLines):
        if a.rstrip() != b.rstrip():
            return False
    return True

if __name__ == "__main__":
    try:
        workingDir = sys.argv[1]
        bextractLocation = sys.argv[2]
        sfpluginLocation = sys.argv[3]
        extraBextractCmdArgs = sys.argv[4]
        mfFileLocation = sys.argv[5]
        outputMplFile = sys.argv[6]
        testFile = sys.argv[7]
        goodFilename = sys.argv[8]
    except:
        print "Syntax: cmake-test-wrapper.py WORKING_DIR ENV_VARS CMD ARG1 ARG2 ... ARGn "
        sys.exit(0)

    returnCode = runCommand(workingDir, bextractLocation, sfpluginLocation, extraBextractCmdArgs,
                            mfFileLocation, outputMplFile, testFile, goodFilename)

    sys.exit(returnCode)

