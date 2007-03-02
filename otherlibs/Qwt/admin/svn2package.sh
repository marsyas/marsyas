#! /bin/sh
# 
# Generates a Qwt package from sourceforge svn
#
# Usage: svn2package.sh [-b|--branch <svn-branch>] [packagename] 
# 

##########################
# usage
##########################

function usage() {
    echo "Usage: $0 [-b|--branch <svn-branch>] [packagename]"
    exit 1
}

################################
# checkout 
################################

function checkoutQwt() {

    if [ -x $2 ]
    then
        rm -r $2
        if [ $? -ne 0 ]
        then
            exit $?
        fi
    fi

    svn -q co https://qwt.svn.sourceforge.net/svnroot/qwt/$1/$2
    if [ $? -ne 0 ]
    then
        echo "Can't access sourceforge SVN"
        exit $?
    fi

    if [ "$3" != "$2" ]
    then
        rm -rf $3
        mv $2 $3
    fi
}

##########################
# cleanQwt dirname
##########################

function cleanQwt {

    cd $1
    if [ $? -ne 0 ]
    then
        exit $?
    fi

    find . -name .svn -print | xargs rm -r

    rm admin/svn2package.sh

    PROFILES="qwtconfig.pri"
    for PROFILE in $PROFILES
    do
        sed -e 's/= debug/= release/' $PROFILE > $PROFILE.sed
        mv $PROFILE.sed $PROFILE
    done

    HEADERS=`find . -type f -name '*.h' -print`
    SOURCES=`find . -type f -name '*.cpp' -print`
    PROFILES=`find . -type f -name '*.pro' -print`
    PRIFILES=`find . -type f -name '*.pri' -print`

    for EXPANDFILE in $HEADERS $SOURCES $PROFILES $PRIFILES
    do
        expand -4 $EXPANDFILE > $EXPANDFILE.expand
        mv $EXPANDFILE.expand $EXPANDFILE
    done

    for SRCFILE in $SOURCES $PROFILES $PRIFILES
    do
    	sed -e '/#warning/d' $SRCFILE > $SRCFILE.sed
        mv $SRCFILE.sed $SRCFILE
    done 

    cd -
}

##########################
# createDocs dirname
##########################

function createDocs {

    ODIR=`pwd`

    cd $1
    if [ $? -ne 0 ]
    then
        exit $?
    fi

    # We need LateX for the qwtdoc.pdf

    sed -e '/GENERATE_LATEX/d' -e '/GENERATE_MAN/d' -e '/PROJECT_NUMBER/d' Doxyfile > Doxyfile.doc
    echo 'GENERATE_LATEX = YES' >> Doxyfile.doc
    echo 'GENERATE_MAN = YES' >> Doxyfile.doc
    echo "PROJECT_NUMBER = $VERSION" >> Doxyfile.doc

    cp ../INSTALL ../COPYING ./

    doxygen Doxyfile.doc > /dev/null
    if [ $? -ne 0 ]
    then
        exit $?
    fi

    rm Doxyfile.doc Doxygen.log INSTALL COPYING
    rm -r images

    cd latex
    make > /dev/null 2>&1
    if [ $? -ne 0 ]
    then 
        exit $?
    fi

    cd ..
    mkdir pdf
    mv latex/refman.pdf pdf/qwtdoc.pdf

    rm -r latex 
    
    cd $ODIR
}

##########################
# posix2dos filename
##########################

function posix2dos {
    # At least one unix2dos writes to stdout instead of overwriting the input.
    # The -q option is always enabled in stdin->stdout mode.
    unix2dos <$1 >$1.dos
    mv $1.dos $1
}

##########################
# prepare4Win dirname
##########################

function prepare4Win {

    cd $1
    if [ $? -ne 0 ]
    then
        exit $?
    fi

    rm -r doc/man 

    # win files, but not uptodate

    BATCHES=`find . -type f -name '*.bat' -print`
    HEADERS=`find . -type f -name '*.h' -print`
    SOURCES=`find . -type f -name '*.cpp' -print`
    PROFILES=`find . -type f -name '*.pro' -print`
    PRIFILES=`find . -type f -name '*.pri' -print`

    for FILE in $BATCHES $HEADERS $SOURCES $PROFILES $PRIFILES
    do
        posix2dos $FILE
    done

    cd -
}

##########################
# prepare4Unix dirname
##########################

function prepare4Unix {

    cd $1
    if [ $? -ne 0 ]
    then
        exit $?
    fi

    rm -rf admin

    cd -
}

##########################
# main
##########################

QWTDIR=
SVNDIR=trunk
BRANCH=qwt
VERSION=

while [ $# -gt 0 ] ; do
    case "$1" in
        -h|--help)
            usage; exit 1 ;;
        -b|--branch)
            shift; SVNDIR=branches; BRANCH=$1; shift;;
        *) 
            QWTDIR=qwt-$1 ; VERSION=$1; shift;;
    esac
done

if [ "$QWTDIR" == "" ] 
then 
	usage 
	exit 2 
fi

TMPDIR=/tmp/$QWTDIR-tmp

echo -n "checkout to $TMPDIR ... "
checkoutQwt $SVNDIR $BRANCH $TMPDIR
cleanQwt $TMPDIR
echo done

echo -n "generate documentation ... "
createDocs $TMPDIR/doc
mv $TMPDIR/doc/pdf/qwtdoc.pdf $QWTDIR.pdf
rmdir $TMPDIR/doc/pdf
echo done


DIR=`pwd`
echo -n "create packages in $DIR ... "

cd /tmp

rm -rf $QWTDIR
cp -a $TMPDIR $QWTDIR
prepare4Unix $QWTDIR
tar cfz $QWTDIR.tgz $QWTDIR
tar cfj $QWTDIR.tar.bz2 $QWTDIR

rm -rf $QWTDIR
cp -a $TMPDIR $QWTDIR
prepare4Win $QWTDIR
zip -r $QWTDIR.zip $QWTDIR > /dev/null

rm -rf $TMPDIR $QWTDIR

mv $QWTDIR.tgz $QWTDIR.tar.bz2 $QWTDIR.zip $DIR/
echo done

exit 0
