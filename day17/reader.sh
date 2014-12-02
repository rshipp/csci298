#/bin/bash

# usage$ ./reader.sh thePipe
echo >/dev/tty "Reading..."
while read line ; do 
	echo "read ${line}"
	sleep 1
done < "${1}"

