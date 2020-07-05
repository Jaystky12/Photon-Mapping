IDIR = ./include
CC=g++
LFLAGS=-I$(IDIR) -fopenmp
CFLAGS=-I$(IDIR) -O3 -Wall
SDIR=./source
ODIR=./obj

default: photon_mapping

photon_mapping: $(ODIR)/photon_mapping.o $(ODIR)/triangle.o $(ODIR)/sphere.o $(ODIR)/scene.o $(ODIR)/nearest_photon.o $(ODIR)/math_calc.o $(ODIR)/ray_tracer.o $(ODIR)/photon_map.o
	$(CC) $(LFLAGS) -o photon_mapping $(ODIR)/photon_mapping.o $(ODIR)/triangle.o $(ODIR)/sphere.o $(ODIR)/scene.o $(ODIR)/nearest_photon.o $(ODIR)/math_calc.o $(ODIR)/ray_tracer.o $(ODIR)/photon_map.o

$(ODIR)/photon_mapping.o: $(SDIR)/photon_mapping.cpp
	$(CC) $(CFLAGS) -c $(SDIR)/photon_mapping.cpp  -o $(ODIR)/photon_mapping.o
	
$(ODIR)/triangle.o: $(SDIR)/triangle.cpp
	$(CC) $(CFLAGS) -c $(SDIR)/triangle.cpp -o $(ODIR)/triangle.o
	
$(ODIR)/sphere.o: $(SDIR)/sphere.cpp
	$(CC) $(CFLAGS) -c $(SDIR)/sphere.cpp -o $(ODIR)/sphere.o	

$(ODIR)/scene.o: $(SDIR)/scene.cpp
	$(CC) $(CFLAGS) -c $(SDIR)/scene.cpp -o $(ODIR)/scene.o	

$(ODIR)/nearest_photon.o: $(SDIR)/nearest_photon.cpp
	$(CC) $(CFLAGS) -c $(SDIR)/nearest_photon.cpp -o $(ODIR)/nearest_photon.o		 

$(ODIR)/math_calc.o: $(SDIR)/math_calc.cpp
	$(CC) $(CFLAGS) -c $(SDIR)/math_calc.cpp -o $(ODIR)/math_calc.o	

$(ODIR)/ray_tracer.o: $(SDIR)/ray_tracer.cpp
	$(CC) $(CFLAGS) -c $(SDIR)/ray_tracer.cpp -o $(ODIR)/ray_tracer.o

$(ODIR)/photon_map.o: $(SDIR)/photon_map.cpp
	$(CC) $(CFLAGS) -c $(SDIR)/photon_map.cpp -o $(ODIR)/photon_map.o	


.PHONY: clean
clean:
	rm -f $(ODIR)/*.o photon_mapping
