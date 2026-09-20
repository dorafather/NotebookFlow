
PWD=`pwd | awk '{print $1}'`
TMP=$PWD
echo "Asif  Home = $TMP"
AF_HOME=$TMP

if [ -f "af.mak" ] ;then
	echo "Already Exist File"
else
	echo "AF_HOME=$TMP">>af.mak
	echo "AF_INC = -I${AF_HOME}/data ">>af.mak
	echo "AF_INC += -I${AF_HOME}/json ">>af.mak
	echo "AF_INC += -I${AF_HOME}/string ">>af.mak
	echo "AF_INC += -I${AF_HOME}/thread ">>af.mak
	echo "AF_INC += -I${AF_HOME}/time ">>af.mak
	echo "AF_INC += -I/usr/local/include ">>af.mak
	echo "AF_LIB = -L${AF_HOME}/lib -lasif ">>af.mak
	echo "AF_LIB += -lssl -lcrypto ">>af.mak
	chmod 777 af.mak
fi

make clean
make
make install

