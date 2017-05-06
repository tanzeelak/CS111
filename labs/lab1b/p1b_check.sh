#!/bin/bash
#
# sanity check script for Project 1A
#
LAB="lab1b"
README="README"
MAKEFILE="Makefile"

EXPECTED="my.key"
EXPECTEDS="c"
CLIENT=./lab1b-client
SERVER=./lab1b-server
PGMS="$CLIENT $SERVER"

TIMEOUT=10
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
    echo "SUBMISSION $tarball ... NO SUCH FILE"
    exit 1
fi

# make sure we can untar it
TEMP="/tmp/TestTemp.$$"
echo
echo
echo "... Checking $tarball in temporary testing directory" $TEMP
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
    echo "SUBMISSION $tarball ... DOES NOT UN-TAR"
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

# make sure we find files with all the expected suffixes
echo "... checking for files of expected types"
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
if [ $? -ne 0 ]
then
    echo "ERROR: default make fails"
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
	echo "SUBMISSION $tarball ... DOES NOT BUILD REQUIRED EXECUTABLE $p"
	let errors+=1
	else
	echo "        $p ... OK"
	fi
done

# see if it excepts the expected arguments
function testrc {
    if [ $1 -ne $2 ]
	then
	echo "ERROR: expected RC=$2, GOT $1"
	let errors+=1
	fi
}

for p in $PGMS
do
    echo "... $p detects/reports bogus arguments"
    $p --bogus < /dev/tty > /dev/null 2>STDERR
    testrc $? 1
    if [ ! -s STDERR ]
	then
	echo "No Usage message to stderr for --bogus"
	let errors+=1
	else
	cat STDERR
	fi
done

echo "... $CLIENT detects/reports system unwritable log file"
timeout $TIMEOUT $CLIENT --log=/tmp < /dev/tty > /dev/null 2>STDERR
testrc $? 1
if [ ! -s STDERR ]
then
    echo "No error message to stderr for non-writable log file"
    let errors+=1
else
    cat STDERR
fi

echo "... usage of mcrypt"
for r in mcrypt_module_open mcrypt_generic mdecrypt_generic
do
    grep $r *.c > /dev/null
    if [ $? -ne 0 ] 
	then
	echo "No calls to $r"
	let errors+=1
	else
	echo "    ... $r ... OK"
	fi
done

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
