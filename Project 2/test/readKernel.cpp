#include <string>
#include <fstream>
#include <iostream>
using namespace std;

void readKernel() {
  string line;
  ifstream myfile("file.txt");
  if (myfile)
  {
    while ( myfile.good() )
    {
      cout << (char) myfile.get();
    }
    myfile.close();
  }

  else cout << "Unable to open file"; 
}

int main() {
	readKernel();
	return 0;
}