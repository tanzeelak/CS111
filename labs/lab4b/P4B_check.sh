#!/bin/bash
#
# sanity check script for Project 2B
#	tarball name
#	tarball contents
#	student identification 
#	makefile targets
#	use of expected functions
#	error free build
#	unrecognized parameters
#	recognizes standard parameters
#
# Note: if you dummy up the sensor sampling
#	this test script can be run on any
#	Linux system.
#
#
LAB="lab4b"
README="README"
MAKEFILE="Makefile"

EXPECTED=""
EXPECTEDS="c"
PGM="lab4b"
PGMS=$PGM

TIMEOUT=5

let errors=0

if [ -z "$1" ]
then
	echo usage: $0 your-student-id
	exit 1
else
	student=$1
fi

# make sure the tarball has the right name
tarball="$LAB-$student.tar.gz"
if [ ! -s $tarball ]
then
	echo "ERROR: Unable to find submission tarball:" $tarball
	exit 1
fi

# make sure we can untar it
TEMP="/tmp/TestTemp.$$"
echo "... Using temporary testing directory" $TEMP
function cleanup {
	cd
	rm -rf $TEMP
	exit $1
}

mkdir $TEMP
cp $tarball $TEMP
cd $TEMP
echo "... untaring" $tarbsll
tar xvf $tarball
if [ $? -ne 0 ]
then
	echo "ERROR: Error untarring $tarball"
	cleanup 1
fi

# make sure we find all the expected files
echo "... checking for expected files"
for i in $README $MAKEFILE $EXPECTED
do
	if [ ! -s $i ]
	then
		echo "ERROR: unable to find file" $i
		let errors+=1
	else
		echo "        $i ... OK"
	fi
done

# make sure the README contains name and e-mail
echo "... checking for submitter info in $README"
function idString {
	result=`grep $1 $README | cut -d: -f2 | tr -d \[:blank:\]`
	if [ -z "$result" ]
	then
		echo "ERROR - no $1 in $README";
		let errors+=1
	elif [ -n "$2" -a "$2" != "$result" ]
	then
		echo "        $1 ... $result != $2"
	else
		echo "        $1 ... $result"
	fi
}

idString "NAME:"
idString "EMAIL:"
idString "ID:" $student

function makeTarget {
	result=`grep $1: $MAKEFILE`
	if [ $? -ne 0 ]
	then
		echo "ERROR: no $1 target in $MAKEFILE"
		let errors+=1
	else
		echo "        $1 ... OK"
	fi
}

echo "... checking for expected make targets"
makeTarget "clean"
makeTarget "dist"
makeTarget "check"

# make sure we find files with all the expected suffixes
echo "... checking for other files of expected types"
for s in $EXPECTEDS
do
	names=`echo *.$s`
	if [ "$names" = '*'.$s ]
	then
		echo "ERROR: unable to find any .$s files"
		let errors+=1
	else
		for f in $names
		do
			echo "        $f ... OK"
		done
	fi
done

# make sure we can build the expected program
echo "... building default target(s)"
make 2> STDERR
RET=$?
if [ $RET -ne 0 ]
then
	echo "ERROR: default make fails RC=$RET"
	let errors+=1
fi
if [ -s STDERR ]
then
	echo "ERROR: make produced output to stderr"
	let errors+=1
fi

echo "... checking for expected products"
for p in $PGMS
do
	if [ ! -x $p ]
	then
		echo "ERROR: unable to find expected executable" $p
		let errors+=1
	else
		echo "        $p ... OK"
	fi
done

# see if it accepts the expected arguments
function testrc {
	if [ $1 -ne $2 ]
	then
		echo "ERROR: expected RC=$2, GOT $1"
		let errors+=1
	fi
}

# see if they detect and report invalid arguments
for p in $PGMS
do
	echo "... $p detects/reports bogus arguments"
	if [ -x $p ]
	then
		./$p --bogus > /dev/null 2>STDERR
		testrc $? 1
		if [ ! -s STDERR ]
		then
			echo "No Usage message to stderr for --bogus"
			let errors+=1
		else
			cat STDERR
		fi
	else
		echo "Program not found!"
		let errors+=1
	fi
done

# test the standard arguments
p=2
s="C"
echo "... $PGM supports --scale, --perios, --log"
./$PGM --period=$p --scale=$s --log="LOGFILE" <<-EOF
SCALE=F
PERIOD=1
START
STOP
OFF
EOF
ret=$?
if [ $ret -ne 0 ]
then
	echo "RETURNS RC=$ret"
	let errors+=1
fi

if [ ! -s LOGFILE ]
then
	echo "did not create a log file"
	let errors+=1
else
	echo "... $PGM supports and logs all sensor commands"
	for c in SCALE=F PERIOD=1 START STOP OFF SHUTDOWN
	do
		grep $c LOGFILE > /dev/null
		if [ $? -ne 0 ]
		then
			echo "DID NOT LOG $c command"
			let errors+=1
		else
			echo "   ... $c: OK"
		fi
	done

	if [ $errors -gt 0 ]
	then
		echo "   LOG FILE DUMP FOLLOWS   "
		cat LOGFILE
	fi
fi

echo "... correct reporting format"
egrep '[0-9][0-9]:[0-9][0-9]:[0-9][0-9] [0-9][0-9].[0-9]' LOGFILE > /dev/null
if [ $? -eq 0 ] 
then
	echo "... OK"
else
	echo NO VALID REPORTS IN LOGFILE:
	let errors+=1
	cat LOGFILE
fi

# that's all the tests I could think of
echo
if [ $errors -eq 0 ]; then
	echo "SUBMISSION $tarball ... passes sanity check"
	echo
	echo
	cleanup 0
else
	echo "SUBMISSION $tarball ... fails sanity check with $errors errors!"
	echo
	echo
	cleanup -1
fi
