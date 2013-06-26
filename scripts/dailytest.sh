#!/bin/bash
######### update as necessary
gitDir=$1
testDir=$2

print_usage() {
	echo "-- Usage: dailytest <git dir> <test dir>"
}

if [[ (-z $gitDir) ]]; then
	print_usage
	echo "!! Git directory not given!"
	exit
fi

if [[ (-z $testDir) ]]; then
	print_usage
	echo "!! Test directory not given!"
	exit
fi

if [[ !($gitDir =~ ^/) || !($testDir =~ ^/) ]]; then
	print_usage
	echo "!! Directories must be given as absolute paths!"
	exit
fi

#coffeeDir=~/marsyas-coffee

######### non-changing definitions
buildDir=$testDir/build
logDir=$testDir/logs
logBase=$logDir/`date +%y%m%d`

gitLog=$logBase-git.log
configLog=$logBase-config.log
buildLog=$logBase-build.log
testingLog=$logBase-testing.log
sanityLog=$logBase-sanity.log
continuousLog=$logBase-continuous.log
#coffeeLog=$logBase-coffee.log
distLog=$logBase-dist.log
distcheckLog=$logBase-distcheck.log

manualsLog=$logBase-manuals.log
doxyLog=$logBase-doxy.log

report=$logBase-report.txt
lastGoodVersion=$testDir/lastworking.txt

report() {
	echo $1 | tee -a $report
}

#  $1 is the "pass/fail" on the subject line
sendreport() {
	subject="$subjectBase $1"
#	echo "$subject"
#	cat $report
	if [ `which mail` ]
	then
		mail -s "$subject" gtzan@cs.uvic.ca < $report
		mail -s "$subject" graham@percival-music.ca < $report
		mail -s "$subject" sness@sness.net < $report
		mail -s "$subject" lgmartins@users.sourceforge.net < $report
		mail -s "$subject" jakob.leben@gmail.com < $report
	fi
}

#  $1 is the command
#  $2 is the log file for the command
#  $3 is the step name (ie Build, Sanity, Coffee, Dist)

testthing() {
# post command
echo ">> $1"

# report command
echo >> $2
echo ">> $1" >> $2
echo >> $2

# execute command
$1 &>> $2
PASS=$?

if [ "$PASS" = "0" ]
then
	report "-- $3: OK"
else
	report "!! $3: FAILED!"
	last=`cat $lastGoodVersion 2>/dev/null`
	report "!! Last good build: $last"
	echo >> $report
	echo >> $report
	tail -n 50 $2 >> $report
	sendreport "FAILED"
	exit
fi

}



######## actual script


### setup clean dir
mkdir -p $testDir

rm -rf $buildDir
mkdir -p $buildDir

### setup report
rm -rf $logDir
mkdir -p $logDir

report "== Marsyas Automatic Test =="
echo >> $report

### get latest Git
cd $gitDir

testthing "git pull --ff-only" $gitLog "Git update"

version=`git rev-parse HEAD`

report "-- Git revision: $version"
subjectBase="Marsyas Auto-Tester ("`date +%y%m%d`", rev $version):"

### Do tests
cd $buildDir

testthing "cmake $gitDir -DMARSYAS_TESTS=ON" $configLog "CMake"

testthing make $buildLog "Build"

## re-use the name "sanity"
testthing "make test" $testingLog "Testing"
#testthing "scripts/regtest_sanity.py" $sanityLog Sanity
#testthing "scripts/regtest_coffee.py $coffeeDir" $coffeeLog Coffee
#testthing "make Continuous" $continuousLog Continuous

## comment out doc building for now, as doxygen is not installed:
#testthing "make doxy" $doxyLog "Doxygen docs"
#testthing "make docs" $manualsLog "Manuals"

#testthing "make dist" $distLog "Make dist"

#testthing "make distcheck" $distcheckLog "Make distcheck"

sendreport "Pass"
echo $version > $lastGoodVersion

