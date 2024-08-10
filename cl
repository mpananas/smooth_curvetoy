clear
flags="-Wall -g"
gcc $flags -c main.c
gcc $flags -c curve_graphic.c
gcc -o p main.o curve_graphic.o -lm
rm *.o
if [ "$1" == 'e' ]; then
valgrind --leak-check=yes --log-file=valgrind.rpt ./p;
display img.ppm;
fi
