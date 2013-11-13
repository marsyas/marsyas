#!/bin/bash
######### update as necessary
gitDir=$1
testDir=$2
scriptDir=`dirname $0`

print_usage() {
  echo "-- Usage: dailytest <git dir> <test dir>"
}

if [[ (-z $gitDir) ]]; then
  print_usage
  echo "!! Git directory not given!"
  exit 1
fi

if [[ (-z $testDir) ]]; then
  print_usage
  echo "!! Test directory not given!"
  exit 1
fi

if [[ !($gitDir =~ ^/) || !($testDir =~ ^/) ]]; then
  print_usage
  echo "!! Directories must be given as absolute paths!"
  exit 1
fi

#coffeeDir=~/marsyas-coffee

######### non-changing definitions
buildDir=$testDir/build
logDir=$testDir/logs
report=$logDir/`date +%y%m%d`-report.txt
lastGoodVersionFile=$testDir/lastworking.txt

logBase=$logDir/`date +%y%m%d`
gitLog=$logBase-git.log

report() {
  echo $1 | tee -a $report
}

#  $1 is the command
#  $2 is the log file for the command
#  $3 is the step name (ie Build, Sanity, Coffee, Dist)

testthing() {
  # print command
  echo ">> $1"

  # log command
  echo >> $2
  echo ">> $1" >> $2
  echo >> $2

  # execute command
  $1 >> $2 2>&1
  PASS=$?
  if [ $PASS = 0 ]
  then
    report "-- $3: OK"
  else
    report "!! $3: FAILED!"
    report "!! Last up-to-50 lines of output from failing command:"
    report ""
    tail -n 50 $2
    tail -n 50 $2 >> $report
    exit 1
  fi
}

test_configuration() {
  bash "$scriptDir/dailytest_configuration.sh" "$1" "$2" "$3" "$4"
}

######## actual script

last_good_version=`cat $lastGoodVersionFile`

### setup clean dir
mkdir -p $testDir

### setup report
rm -rf $logDir
mkdir -p $logDir

report "== Marsyas Automatic Test =="
report "" # empty line

### get latest Git
cd $gitDir

testthing "git pull --ff-only" $gitLog "Git update"

version=`git rev-parse HEAD`

report "-- Git revision: $version"

# Check for new commits, exit if none.
if [ "$version" = "$last_good_version" ]
then
  report "-- No new git commits."
  exit 0
fi

# Run configuration tests

subjectBase="Marsyas Auto-Tester ("`date +%y%m%d`", rev $version):"

report ""

PASS=1

test_configuration "Default" "$gitDir" "$testDir" \
|| PASS=0

report ""

test_configuration "Minimal" "$gitDir" "$testDir" \
"-DWITH_CPP11=OFF -DMARSYAS_AUDIOIO=OFF -DMARSYAS_MIDIIO=OFF" \
|| PASS=0

report ""

test_configuration "Complete" "$gitDir" "$testDir" \
"-DWITH_CPP11=ON -DWITH_QT=ON -DWITH_OPENGL=ON -DWITH_SWIG=ON -DWITH_SWIG_PYTHON=ON \
 -DMARSYAS_AUDIOIO=ON -DMARSYAS_MIDIIO=ON" \
|| PASS=0

report ""


if [ $PASS = 1 ]
then
  echo $version > $lastGoodVersionFile
  report "-- All configurations have passed."
  emailSubject="$subjectBase PASS"
else
  report "!! Some configurations have failed."
  report "!! Last good version: $last_good_version"
  emailSubject="$subjectBase FAIL"
fi

emailRecipients="\
gtzan@cs.uvic.ca \
graham@percival-music.ca \
sness@sness.net \
lgmartins@users.sourceforge.net \
jakob.leben@gmail.com"

if [ `which mail` ]
then
  #echo "Would send email report to: $emailRecipients"
  mail -s "$emailSubject" $emailRecipients < $report
fi

