3D Game of Life : Parallel programming with Pthreads
===============
To compile program just use provided Makefile (with make command)
Output of compilation is file named test_pthreads.
To run program use compiled program and insert data to stdin from file
Ex.: ./test_pthreads < input-100.life
If everything will proceed correctly output will be one line on stdout
Ex.: Time : 2.12200000                                                
and output.life life which contains computed data.


P.S: We have to be honest at this moment. Out represantation of sequantial version
is really hard to parallel. Actually there is no gain from parallelization since
every effective operation (Means variable = variable2, not for loops etc)
needs to be save and is global. Plus every oeffective operation is dependant on 
the others so that we cant make local update and join it together with global 
variable (Actually this is possible but with HUGE memmory overhead). Hence every 
operation has to be save, this means put every action of this type into mutex or 
another kind of lock. Overhead of these operation is much higher than gain from
parallelization. As a result of all above, we decided to submit two same versions 
(test_seq and test_pthreads are same). To show that we really tried to implement
even the parallel version there is another file in this directory - game_of_life_pthreads_threads.cpp.
To compile this file use command
g++ -g -Wall -o test_real_thread_version game_of_life_pthreads_threads.cpp -lpthreads
result is stored in test_real_thread_version file. 
To run program use compiled program and insert data to stdin from file
Ex.: ./test_real_thread_version file < input-100.life

Since there is no explicitly said we have to use pthread, we hope that 
no use of pthreads in first program is not a problem.
