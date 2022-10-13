
DIR=$1
STR=$2
N=$3

./finder.sh $DIR $STR $N > desired.txt
./finder $DIR $STR $N > output.txt

# output the diff
diff desired.txt output.txt

# clean up outputted text files
rm -f desired.txt output.txt
