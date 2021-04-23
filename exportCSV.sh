##
##  exportCSV.sh
##  ----------------------------------------------
##
##  Author : Gilles Henrard
##  Last edit : 23/04/2021
##
##  Script which runs bin/readerswriters 10 times, counts the amount of times
##    readers had the occasion to write an output (as opposed to the writers),
##    and export all the results in a CSV file in order to use the data in a
##    statistical analysis.
##
##  Its intent is to prove whether or not the readers/writers implementation is
##    satisfactory, and if assigning different priorities to the threads makes
##    any difference.

#!/bin/bash

#create the directory in which put the files
mkdir -p stat

#to be ran 10 times
for i in {1..100} ; do
    #run readerswriters without nice and export the output in a temp file
    echo "without scheduling, run #$i"
    bin/readerswriters 19 4 20 none > stat/rw_nosched_19-4-20.txt

    #run readerswriters with nice and export the output in a temp file
    echo "with nice(), run #$i"
    bin/readerswriters 19 4 20 nice 1 10 > stat/rw_nice_19-4-20.txt

    #run readerswriters with nice and export the output in a temp file
    echo "with FIFO, run #$i"
    bin/readerswriters 19 4 20 fifo > stat/rw_fifo_19-4-20.txt

    #compute the amount of readers outputs with and without nice and append both in CSV files
    cat stat/rw_nosched_19-4-20.txt | grep -i r | wc -l >> stat/rw_nosched_19-4-20.csv
    cat stat/rw_nice_19-4-20.txt | grep -i r | wc -l >> stat/rw_nice_19-4-20.csv
    cat stat/rw_fifo_19-4-20.txt | grep -i r | wc -l >> stat/rw_fifo_19-4-20.csv
done

#delete temp files
rm stat/rw_nosched_19-4-20.txt
rm stat/rw_nice_19-4-20.txt
rm stat/rw_fifo_19-4-20.txt