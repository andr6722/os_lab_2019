#!/bin/bash

touch numbers.txt

for ((i =1; i<=150; i++))
do 
	random_number=$(od -An -N2 -i /dev/random | awk '{print $1 % 10000}')
	echo $random_number >> numbers.txt
	
done 


