#!/bin/sh
#######################################################################
## - acceptance-test.sh
## - Assignment 2 - Metronome
## - @author Mostapha A
#######################################################################

#Unit Test A
echo "********************************************\n"
echo "Unit Test [A] \n./metronome"
echo "Expected: usage message\n"
./metronome 
sleep 1
echo "\n********************************************\n\n"


#Unit Test B
echo "********************************************\n"
echo "Unit Test [B] \n./metronome 120 2 4 &"
echo "Expected: metronome runs at [metronome: 120 beats/min, time signature 2/4, secs-per-interval: 0.25, nanoSecs: 250000000]\n"
./metronome 120 2 4 &
sleep 5
echo "\n********************************************\n\n"

#Unit Test C
echo "********************************************\n"
echo "Unit Test [C] \ncat /dev/local/metronome"
echo "Expected: [metronome: 120 beats/min, time signature 2/4, secs-per-interval: 0.25, nanoSecs: 250000000]\n"
cat /dev/local/metronome
sleep 1
echo "\n********************************************\n\n"

#Unit Test D
echo "********************************************\n"
echo "Unit Test [D] \ncat /dev/local/metronome-help"
echo "Expected: information regarding the metronome resmgr’s API\n"
cat /dev/local/metronome-help
sleep 1
echo "\n********************************************\n\n"

#Unit Test E
echo "********************************************\n"
echo "Unit Test [E] \necho set 100 2 4 > /dev/local/metronome"
echo "Expected: metronome regmgr changes settings to: 100 bpm in 2/4 time; run-time behaviour of metronome changes to 100 bpm in 2/4 time.\n"
echo set 100 2 4 > /dev/local/metronome
sleep 5
echo "\n********************************************\n\n"

#Unit Test F
echo "********************************************\n"
echo "Unit Test [F] \ncat /dev/local/metronome"
echo "Expected: [metronome: 100 beats/min, time signature 2/4, secs-per- interval: 0.30, nanoSecs: 300000000]\n"
cat /dev/local/metronome
sleep 1
echo "\n********************************************\n\n"

#Unit Test G
echo "********************************************\n"
echo "Unit Test [G] \necho set 200 5 4 > /dev/local/metronome"
echo "Expected: metronome regmgr changes settings to: 200 bpm in 5/4 time; run-time behaviour of metronome changes to 200 bpm in 5/4 time.\n"
echo set 200 5 4 > /dev/local/metronome
sleep 5
echo "\n********************************************\n\n"

#Unit Test H
echo "********************************************\n"
echo "Unit Test [H] \ncat /dev/local/metronome"
echo "Expected: [metronome: 200 beats/min, time signature 5/4, secs-per- interval: 0.15, nanoSecs: 150000000]\n"
cat /dev/local/metronome
sleep 1
echo "\n********************************************\n\n"

#Unit Test I
echo "********************************************\n"
echo "Unit Test [I] \necho stop > /dev/local/metronome"
echo "Expected: metronome stops running; metronome resmgr is still running as a process: pidin | grep metronome\n"
echo stop > /dev/local/metronome
echo "\n"
pidin | grep metronome
sleep 5
echo "\n********************************************\n\n"

#Unit Test J
echo "********************************************\n"
echo "Unit Test [J] \necho start > /dev/local/metronome"
echo "Expected: metronome starts running again at 200 bpm in 5/4 time, which is the last setting; metronome resmgr is still running as a process: pidin | grep metronome\n"
echo "\n"
pidin | grep metronome
sleep 5
echo "\n********************************************\n\n"

#Unit Test K
echo "********************************************\n"
echo "Unit Test [K] \ncat /dev/local/metronome"
echo "Expected: [metronome: 200 beats/min, time signature 5/4, secs-per- interval: 0.15, nanoSecs: 150000000]\n"
cat /dev/local/metronome
sleep 5
echo "\n********************************************\n\n"

#Unit Test L
echo "********************************************\n"
echo "Unit Test [L] \necho stop > /dev/local/metronome"
echo "Expected: metronome stops running; metronome resmgr is still running as a process: pidin | grep metronome.\n"
echo stop > /dev/local/metronome
echo "\n"
pidin | grep metronome
sleep 5
echo "\n********************************************\n\n"

#Unit Test M
echo "********************************************\n"
echo "Unit Test [M] \necho stop > /dev/local/metronome"
echo "Expected: metronome remains stopped; metronome resmgr is still running as a process: pidin | grep metronome.\n"
echo stop > /dev/local/metronome
echo "\n"
pidin | grep metronome
sleep 5
echo "\n********************************************\n\n"

#Unit Test N
echo "********************************************\n"
echo "Unit Test [N] \necho start > /dev/local/metronome"
echo "Expected: metronome starts running again at 200 bpm in 5/4 time, which is the last setting; metronome resmgr is still running as a process: pidin | grep metronome\n"
echo start > /dev/local/metronome
echo "\n"
pidin | grep metronome
sleep 5
echo "\n********************************************\n\n"

#Unit Test O
echo "********************************************\n"
echo "Unit Test [O] \necho start > /dev/local/metronome"
echo "Expected: metronome is still running again at 200 bpm in 5/4 time, which is the last setting; metronome resmgr is still running as a process: pidin | grep metronome\n"
echo start > /dev/local/metronome
echo "\n"
pidin | grep metronome
sleep 5
echo "\n********************************************\n\n"

#Unit Test P
echo "********************************************\n"
echo "Unit Test [P] \ncat /dev/local/metronome"
echo "Expected: [metronome: 200 beats/min, time signature 5/4, secs-per- interval: 0.15, nanoSecs: 150000000]\n"
cat /dev/local/metronome
sleep 1
echo "\n********************************************\n\n"

#Unit Test Q
echo "********************************************\n"
echo "Unit Test [Q] \necho pause 3 > /dev/local/metronome"
echo "Expected: metronome continues on next beat (not next measure).\n"
echo pause 3 > /dev/local/metronome
sleep 5
echo "\n********************************************\n\n"

#Unit Test R
echo "********************************************\n"
echo "Unit Test [R] \necho pause 10 > /dev/local/metronome"
echo "Expected: properly formatted error message, and metronome continues to run.\n"
echo pause 10 > /dev/local/metronome
sleep 1
echo "\n********************************************\n\n"

#Unit Test S
echo "********************************************\n"
echo "Unit Test [S] \necho bogus > /dev/local/metronome"
echo "Expected: properly formatted error message, and metronome continues to run.\n"
echo bogus > /dev/local/metronome
sleep 1
echo "\n********************************************\n\n"

#Unit Test T
echo "********************************************\n"
echo "Unit Test [T] \necho set 120 2 4 > /dev/local/metronome"
echo "Expected: 1 measure per second\n"
echo set 120 2 4 > /dev/local/metronome
sleep 5
echo "\n********************************************\n\n"

#Unit Test U
echo "********************************************\n"
echo "Unit Test [U] \ncat /dev/local/metronome"
echo "Expected: [metronome: 120 beats/min, time signature 2/4, secs-per-interval: 0.25, nanoSecs: 250000000]\n"
cat /dev/local/metronome
sleep 1
echo "\n********************************************\n\n"

#Unit Test V
echo "********************************************\n"
echo "Unit Test [V] \ncat /dev/local/metronome-help"
echo "Expected: information regarding the metronome resmgr’s API\n"
cat /dev/local/metronome-help
sleep 1
echo "\n********************************************\n\n"

#Unit Test W
echo "********************************************\n"
echo "Unit Test [W] \necho Writes-Not-Allowed > /dev/local/metronome-help"
echo "Expected: properly formatted error message, and metronome continues to run.\n"
echo Writes-Not-Allowed > /dev/local/metronome-help
sleep 1
echo "\n********************************************\n\n"

#Unit Test X
echo "********************************************\n"
echo "Unit Test [X] \necho quit > /dev/local/metronome && pidin | grep metronome"
echo "Expected: metronome gracefully terminates.\n"
echo quit > /dev/local/metronome 
sleep 1
pidin | grep metronome
sleep 1
echo "\n********************************************\n\n"
