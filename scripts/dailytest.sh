#!/bin/bash
######### update as necessary
baseDir=~/usr/src/marsyas
matDir=~/usr/src/marsyas-mat
coffeeDir=~/usr/src/marsyas-coffee


### get latest SVN
cd $baseDir
svn update

######### non-changing definitions
version=`svn info | grep Revision | cut -c 11-`
subjectBase="Marsyas Auto-Tester ("`date +%y%m%d`", rev $version):"

buildDir=$matDir/testbuild
logDir=$matDir/logs
logBase=$logDir/`date +%y%m%d`

configLog=$logBase-config.log
buildLog=$logBase-build.log
sanityLog=$logBase-sanity.log
coffeeLog=$logBase-coffee.log
distLog=$logBase-dist.log
distcheckLog=$logBase-distcheck.log

manualsLog=$logBase-manuals.log
doxyLog=$logBase-doxy.log

report=$logBase-report.txt
lastGoodVersion=$matDir/lastworking.txt

#  $1 is the "pass/fail" on the subject line
sendreport() {
	subject="$subjectBase $1"
	echo "$subject"
	cat $report
	if [ `which mail` ]
	then
		mail -s "$subject" gperciva@uvic.ca < $report
		mail -s "$subject" gtzan@cs.uvic.ca < $report
# can't deliver to this email address
#		mail -s "$subject" lgmartins@users.sourceforge.net < $report
	fi
}

#  $1 is the command
#  $2 is the log file for the command
#  $3 is the step name (ie Build, Sanity, Coffee, Dist)
testthing() {
$1 &> $2
PASS=$?

if [ "$PASS" = "0" ]
then
	echo "$3... succeeded." >> $report
else
	echo "$3... FAILED!   *********" >> $report
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
# play games because make dist marks stuff read-only
chmod -R u+w $buildDir
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

testthing "scripts/regtest_sanity.py" $sanityLog Sanity

testthing "scripts/regtest_coffee.py $coffeeDir" $coffeeLog Coffee

cd doc/
testthing "make html" $manualsLog "HTML manuals"
testthing "make pdf" $manualsLog "PDF manuals"
testthing "make pdf" $doxyLog "Doxygen docs"
cd ..

testthing "make dist" $distLog "Make dist"

testthing "make distcheck" $distcheckLog "Make distcheck"

sendreport "Pass"

