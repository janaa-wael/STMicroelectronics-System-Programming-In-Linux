# Target to build shared library
MyHMM_Lib.so: HMM_pic.o
	gcc --shared -o libhmm.so HMM_pic.o

# Target to build static library
Static_Lib: HMM.o
	ar -rs libhmm.a HMM.o
	gcc -o stress_test stress_test.c libhmm.a

# Target to compile normal object file
HMM.o: HMM.c HMM.h
	gcc -o HMM.o -c HMM.c

# Target to compile position-independent object file (for shared library)
HMM_pic.o: HMM.c HMM.h
	gcc -fPIC -o HMM_pic.o -c HMM.c

# Run the program with LD_LIBRARY_PATH set
run: MyHMM_Lib.so
	LD_LIBRARY_PATH=./ ./stress_test

# Clean target to remove object files and binaries
clean:
	rm -f *.o libhmm.so libhmm.a stress_test

