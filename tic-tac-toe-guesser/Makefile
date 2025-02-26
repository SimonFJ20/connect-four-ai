
CFLAGS = -Wall -Wextra -pedantic -pedantic-errors -Wconversion -fsanitize=address,undefined -g -Og
LIBERALS = -lm

all:
	gcc *.c $(CFLAGS) $(LIBERALS)

release:
	gcc *.c $(CFLAGS) $(LIBERALS) -O3 -flto=auto

plot:
	gnuplot -e 'set style line 1 linecolor rgb "#0060ad" linetype 1 linewidth 2 pointtype 0 pointsize 0.0; set style line 2 linecolor rgb "#60ad00" linetype 1 linewidth 2 pointtype 0 pointsize 0.0; set yrange [0:0.1]; plot "train_loss.dat" with linespoints linestyle 1, "test_loss.dat" with linespoints linestyle 2; pause mouse close'

