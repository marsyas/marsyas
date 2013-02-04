#!/bin/bash
######### update as necessary
baseDir=~/marsyas-svn
matDir=~/marsyas-autotest
#coffeeDir=~/marsyas-coffee


### get latest SVN
cd $baseDir
svn update

######### non-changing definitions
version=`svn info | grep Revision | cut -c 11-`
subjectBase="Marsyas Auto-Tester ("`date +%y%m%d`", rev $version):"

buildDir=$matDir/testbuild
logDir=$matDir/logs
cmakeDir=$buildDir/build
logBase=$logDir/`date +%y%m%d`

configLog=$logBase-config.log
buildLog=$logBase-build.log
sanityLog=$logBase-sanity.log
continuousLog=$logBase-continuous.log
#coffeeLog=$logBase-coffee.log
distLog=$logBase-dist.log
distcheckLog=$logBase-distcheck.log

manualsLog=$logBase-manuals.log
doxyLog=$logBase-doxy.log

report=$logBase-report.txt
lastGoodVersion=$matDir/lastworking.txt

#  $1 is the "pass/fail" on the subject line
sendreport() {
	subject="$subjectBase $1"
#	echo "$subject"
	cat $report
	if [ `which mail` ]
	then
		mail -s "$subject" gtzan@cs.uvic.ca < $report
		mail -s "$subject" graham@percival-music.ca < $report
		mail -s "$subject" sness@sness.net < $report
		mail -s "$subject" lgmartins@users.sourceforge.net < $report
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
	last=`cat $lastGoodVersion`
	echo "Last good build... $last" >> $report
	echo >> $report
	echo >> $report
	tail -n 50 $2 >> $report
	sendreport "FAILED"
	exit
fi

}



######## actual script

### setup clean dir
mkdir -p $matDir
rm -rf $buildDir
cd $baseDir
#svn co https://svn.code.sf.net/p/marsyas/code/trunk marsyas
svn export . $buildDir
mkdir -p $cmakeDir


### setup report
rm -rf $logDir
mkdir -p $logDir
echo "Marsyas Automatic Test, svn revision $version" >> $report
echo >> $report


### Do tests
cd $cmakeDir
cmake ../src/ -DMARSYAS_TESTS=ON &> $configLog

testthing make $buildLog "Build"

## re-use the name "sanity"
testthing "make test" $sanityLog Sanity
#testthing "scripts/regtest_sanity.py" $sanityLog Sanity
#testthing "scripts/regtest_coffee.py $coffeeDir" $coffeeLog Coffee
#testthing "make Continuous" $continuousLog Continuous

cd ..
mkdir doc-build
cd doc-build
cmake ../doc/
#testthing "make sources" $doxyLog "source-highlight examples"
testthing "make doxy" $doxyLog "Doxygen docs"
testthing "make" $manualsLog "Manuals"
#testthing "make html" $manualsLog "HTML manuals"
cd ..

#testthing "make dist" $distLog "Make dist"

#testthing "make distcheck" $distcheckLog "Make distcheck"

sendreport "Pass"
echo $version > $lastGoodVersion

