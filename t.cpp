#include <iostream>
#include <fstream>
#include <string>
int main()
{
    std::cout << "Hello world!" << std::endl;
    std::ifstream infilestream;

	std::string line;
    
    infilestream.open("shaders.h");
    while(infilestream)
    {
        std::getline(infilestream, line);
        std::cout<<line<<"\n";
    }
    infilestream.close();
    return 0;
}