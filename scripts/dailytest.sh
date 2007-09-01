#!/bin/sh
######### update as necessary
baseDir=~/usr/src/marsyas
matDir=~/usr/src/marsyas-mat


######### non-changing definitions
cd $baseDir
version=`svn info | grep Revision | cut -c 11-`
subjectBase="Marsyas Auto-Tester ("`date +%y%m%d`", rev $version):"

buildDir=$matDir/testbuild
logDir=$matDir/logs
logBase=$logDir/`date +%y%m%d`

configLog=$logBase-config.log
buildLog=$logBase-build.log
sanityLog=$logBase-sanity.log
coffeeLog=$logBase-coffee.log

report=$logBase-report.txt
lastGoodVersion=$matDir/lastworking.txt

#  $1 is the "pass/fail" on the subject line
sendreport() {
	subject="$subjectBase $1"
	echo "$subject"
	cat $report
	mail -s "$subject" gperciva@uvic.ca < $report
}

#  $1 is the command
#  $2 is the log file for the command
#  $3 is the step name (ie Build, Sanity, Coffee, Dist)
testthing() {
$1 &> $2
PASS=$?

if [ "$PASS" = "0" ]
then
	echo "$3 succeeded..." >> $report
else
	echo "$3 FAILED!   *********" >> $report
	echo >> $report
	tail -n 50 $2 >> $report
	sendreport "FAILED"
	exit
fi

}



######## actual script

### setup clean dir
mkdir -p $matDir
cd $baseDir
rm -rf $buildDir
svn export . $buildDir


### setup report
rm -rf $logDir
mkdir -p $logDir
echo "Marsyas Automatic Test, svn revision $version" >> $report
echo >> $report


### Do tests
cd $buildDir
./configure &> $configLog
testthing make $buildLog "Build"

testthing "scripts/regtest_sanity.py" $sanityLog Commit

testthing "scripts/regtest_coffee.py ../../marsyas-coffee" $coffeeLog Coffee

sendreport "Pass"

