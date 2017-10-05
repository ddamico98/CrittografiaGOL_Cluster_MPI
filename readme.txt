To use this application you must install some library, in this way :

1) Open linux Bash;
2) sudo -s				  To became superuser
3) apt-get install liballegro4.4          Install the graphics library Allegro
4) apt-get install liballegro4-dev 
5) apt-get install openmpi-*              Install the mpi library to use the cluster
6) apt-get install libopenmpi-dev 

To execute :

7) mpic++ golmpi.cpp -o golmpi `allegro-config --libs` -fpermissive -lm

8) mpirun ./golmpi -np 4                   Run the application in a virtual cluster with 4 node 
