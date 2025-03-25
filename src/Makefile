#
#   ALL RIGHTS RESERVED.
#   _________________________________________________________________________________
#   NOTICE: All information contained  herein is, and remains the property  of  Varga
#   Consulting and  its suppliers, if  any. The intellectual and  technical  concepts
#   contained herein are proprietary to Varga Consulting and its suppliers and may be
#   covered  by  Canadian and  Foreign Patents, patents in process, and are protected
#   by  trade secret or copyright law. Dissemination of this information or reproduc-
#   tion  of  this  material is strictly forbidden unless prior written permission is
#   obtained from Varga Consulting.
#
#   Copyright © <2017-2025> Varga Consulting, Toronto, On     info@vargaconsulting.ca
#   _________________________________________________________________________________

app = iex2h5

ARMA_CFLAGS = -DARMA_DONT_USE_WRAPPER -DARMA_USE_BLAS -DARMA_USE_LAPACK -DARMA_USE_HDF5 -DARMA_64BIT_WORD
MPI = -I/usr/local/include -pthread -L/usr/local/lib -Wl,-rpath -Wl,/usr/local/lib -Wl,-rpath -Wl,/usr/local/lib -Wl,--enable-new-dtags -L/usr/local/lib -lmpi_cxx -lmpi
ARMA = `pkg-config --libs armadillo`
MKL = -Wl,--start-group ${MKLROOT}/lib/intel64/libmkl_intel_ilp64.a ${MKLROOT}/lib/intel64/libmkl_sequential.a ${MKLROOT}/lib/intel64/libmkl_core.a -Wl,--end-group -lpthread -lm -ldl
HDF5 = `pkg-config --libs hdf5`

LIBS =      -lglog  -lprofiler -lpcap -pthread -ldate-tz -lcurl     \
			-lboost_program_options -lboost_system 					\
			-lm -lz  -ldl -lpthread 							    \
			$(ARMA)  $(MKL) $(HDF5)						       		\

CXXFLAGS =  -I ../../include -I/usr/local/include `pkg-config --cflags hdf5` -std=c++17  		\
			-DARMA_DONT_USE_WRAPPER 								\
			-DHOST_NAME_MAX=254 									\
			-DVALUE_PRECISION=6 -DSTAT_PRECISION=4  				\
			-DNODEBUG -DIEX_MAX_SYMBOLS=20000 -g -O3 -no-pie

%.o : $(SRC_DIR)/%.cpp 
	$(CXX) -$(INCLUDES) -o $@  $(CPPFLAGS) $(CXXFLAGS) -c $^

iex2h5: $(app).o consumer_asset.o consumer_irts.o consumer_rts.o consumer_index.o
	$(CXX) $^ $(LIBS) -Wl,-rpath=$(pwd)  -o $@ 	

all: $(app) 

install:
	cp $(app) /usr/local/bin/

clean:
	@$(RM) *.o *.prof  $(app)
