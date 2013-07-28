#include <stdio.h>
#include <string.h>
#include "lev_dist.h"

#include <iostream>
#include <string>

int main(void)
{
  int buffer[1024];


  while(std::cin.good()) {
    std::string line;
    std::getline(std::cin, line);
    
    size_t pos = line.find('\t');
    std::string s1, s2;
    if (pos == std::string::npos) {
      s1 = line;
    } else {
      size_t pos_end = line.find('\t', pos + 1);
      s1 = line.substr(0, pos);
      s2 = line.substr(pos + 1);
    }


    size_t dist = editdist(s1.data(), s2.data(), s1.size(), s2.size(), buffer, 4);
    std::cout << dist << std::endl;
  }


  return 0;
}
