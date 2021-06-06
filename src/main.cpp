////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Dimitry Ishenko
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com
//
// Distributed under the GNU GPL license. See the LICENSE.md file for details.

////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <stdexcept>

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
try
{
    std::cout << "Hello world!" << std::endl;
    return 0;
}
catch(std::exception& e)
{
    std::cerr << e.what() << std::endl;
    return 1;
}
