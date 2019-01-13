#include "easynet_endian.h"
#include <iostream>

using namespace std;
using namespace easynet::sys;
int main()
{
    int64_t little;
    int64_t big = littleEndian2Big((int64_t)12345);

    little = bigEndian2Little(big);

    cout<<"big = "<<big<<endl;
    cout<<"little = "<<little<<endl;
}