IDIR = /usr/local/include
CC = gcc
CFLAGS = -Wall -I$(IDIR)

LDIR = -L/usr/local/lib
LIBS = -lwfdb -lm

DEPS = bf_lib/bf_math.h bf_lib/fifo.h cmplx.h psr.h cpsd.h osea_lib/qrsdet.h

_OBJ = ecg_analysis.o fifo.o bf_math.o cmplx.o psr.o cpsd.o qrsdet.o qrsfilt.o noisechk.o rythmchk.o analbeat.o match.o postclas.o classify.o bdac.o
OBJ = $(patsubst %.o,obj/%.o,$(_OBJ))


#the make would recompile .c file if the .h file is changed
obj/%.o : bf_lib/%.c $(DEPS)
		$(CC) -c -o $@ $< $(CFLAGS)

obj/%.o : osea_lib/%.c $(DEPS)
		$(CC) -c -o $@ $< $(CFLAGS)

obj/%.o : %.c $(DEPS)
		$(CC) -c -o $@ $< $(CFLAGS)

ryth_analysis:$(OBJ)
		$(CC) -o $@ $^ $(CFLAGS) $(LDIR) $(LIBS)

.PHONY: clean
clean:
	rm -rf obj/*.o *~ ryth_analysis 
