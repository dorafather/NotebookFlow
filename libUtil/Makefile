CROSS_BUILD=false
CROSS_TARGET=aarch64-linux-gnu

ifeq ($(CROSS_BUILD),true)
CXX=$(CROSS_TARGET)-g++
AR=$(CROSS_TARGET)-ar
RANLIB=$(CROSS_TARGET)-ranlib
ARCH_FLAGS=
else
ARCH_FLAGS=-march=x86-64
endif

DEFLIST		= -Wall -Wno-deprecated -g -DDEBUG -D_REENTRANT $(ARCH_FLAGS) -fPIC
DEFLIST      += -DUNIX -D_LINUX_ -DLINUX
DEFLIST      += -std=c++11
COMMONLIB	= -lm -ldl -lresolv -lnsl -lstdc++ -lrt -lpthread -lz -lcurl

DEF	= $(WARN) $(DEBUG) $(DEFLIST) 
INC	= -I/usr/local/include
INC += -I./ -I./data -I./json -I./kstring -I./thread -I./dtime -I./parser -I./interpretor -I./flow -I./dfile

SRCS += ./data/ARGV.cpp                                                                                                   
SRCS += ./data/ID.cpp                                                                                                        
SRCS += ./data/LIST.cpp                                                                                                   
SRCS += ./data/SESSION.cpp                                                                                              
SRCS += ./data/STL.cpp  
SRCS += ./data/MATRIX.cpp   
SRCS += ./dfile/DFILE.cpp
SRCS += ./dfile/INIFILE.cpp
SRCS += ./json/JSONK.cpp                                                
SRCS += ./json/JSON_ARRAYLIST.cpp                                       
SRCS += ./json/JSON_DEBUG.cpp                                           
SRCS += ./json/JSON_LINKHASH.cpp                                        
SRCS += ./json/JSON_OBJECT.cpp                                          
SRCS += ./json/JSON_PRINTBUF.cpp                                        
SRCS += ./json/JSON_TOKENER.cpp                                         
SRCS += ./json/JSON_UTIL.cpp                                            
SRCS += ./json/RESTMSG.cpp                                              
SRCS += ./kstring/COMPSTR.cpp
SRCS += ./kstring/STRCONV.cpp
SRCS += ./kstring/KSTRING.cpp
SRCS += ./thread/CONDITION.cpp                                            
SRCS += ./thread/LOCK.cpp                                                 
SRCS += ./thread/QUEUETHREAD.cpp                                          
SRCS += ./thread/THREAD.cpp   
SRCS += ./dtime/GPOLLING.cpp
SRCS += ./dtime/DTIME.cpp
SRCS += ./dtime/TIMER.cpp
SRCS += ./dtime/TIMEUTIL.cpp
SRCS += ./dtime/CRON.cpp
SRCS += ./parser/ACTIONPARSER.cpp
SRCS += ./parser/ASSIGNPARSER.cpp
SRCS += ./parser/BASICPARSER.cpp
SRCS += ./parser/CLASSPARSER.cpp
SRCS += ./parser/CXIFPARSER.cpp
SRCS += ./parser/FUNCPARSER.cpp
SRCS += ./parser/IFPARSER.cpp
SRCS += ./parser/PROCPARSER.cpp
SRCS += ./parser/STATEPARSER.cpp
SRCS += ./parser/STMTPARSER.cpp
SRCS += ./parser/SCEPARSER.cpp
SRCS += ./parser/INIPARSER.cpp
SRCS += ./parser/LOGPARSER.cpp
SRCS += ./parser/SBRKPARSER.cpp
SRCS += ./interpretor/EXECORE.cpp
SRCS += ./interpretor/EXESCEN.cpp
SRCS += ./interpretor/EXEACTION.cpp
SRCS += ./interpretor/EXECXIF.cpp
SRCS += ./interpretor/EXEIF.cpp
SRCS += ./interpretor/EXEPARAM.cpp
SRCS += ./interpretor/EXEPROC.cpp
SRCS += ./interpretor/EXESTATE.cpp
SRCS += ./interpretor/EXESTMT.cpp
SRCS += ./interpretor/EXEFUNC.cpp
SRCS += ./interpretor/EXELOG.cpp
SRCS += ./flow/FLOW.cpp
SRCS += ./flow/MAINP.cpp

OBJS = $(SRCS:.cpp=.o)
.SUFFIES: .cpp .o
TARGET = libdrUtil.a
INSTALL_HOME = ./

all:$(TARGET)

$(TARGET): $(OBJS)
	echo "Link: " $(TARGET)
	ar -rc $@ $(OBJS)

.cpp.o:
	$(CXX) $(DEF) $(INC) -c $< -o $@
	
	
install:
	cp -f $(TARGET) $(INSTALL_HOME)
	rm -f $(TARGET) $(OBJS)

clean:
	rm -f $(OBJS) $(TARGET)
	
