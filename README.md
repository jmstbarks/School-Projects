# OSF15_Project1

Run the ./ps_tester to unit test your process scheduling library
Run the ./ps_analysis to do a integrated test of your process scheduling library

To run the unit test and analysis program. 

cd build
cmake ..
make

./ps_tester <number> 

number {
  1 will test all non schedule functions and First Come First Served
  2 will test all non schedule functions and Shortest Job First
  3 will test all non schedule functions and Shortest Remaining Time First
  4 will test all non schedule functions and Round Robin
}


./ps_analysis <data/binaryTestFile> <number> 

number {
  1 will read the binaryTestFile and run the First Come First Served over the read in data
  2 will read the binaryTestFile and run the Shortest Job First over the read in data
  3 will read the binaryTestFile and run the Shortest Remaining Time First over the read in data
  4 will read the binaryTestFile and run the Round Robin over the read in data
}
