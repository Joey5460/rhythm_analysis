#!/bin/bash
#db_path=/opt/physiobank/database/
db_path=../
db=mitdb
#db=cudb
#db=aha
sr=200
w_len=9
rm -rf detect.out
for filename in ${db_path}/$db/*.dat
do
#    echo "$filename"
    extension="${filename##*.}"
    filename=$(basename ${filename})
    filename="$db/${filename%.*}"
    #echo "$filename">>detect.out
    #./ecg_analysis -p $db_path -r $filename |awk '$1 ==2 && $2 < 100 {print $filename}'>>detect.out
#    echo "$filename" >>detect.out
    echo "$filename" 
    ./ryth_analysis  -p $db_path -r $filename -s $sr -w $w_len >>detect.out
#    ./ryth_analysis  -p $db_path -r $filename -s $sr -w $w_len -d >>detect.out
done
