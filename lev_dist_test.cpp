#include "substr_count.h"
#include "lev_dist.h"
#include "io_tool.h"

#include <iostream>
#include <string>
#include <vector>
#include <assert.h>

#include <stdio.h>
#include <string.h>


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

    std::vector<_lev_state_t> op_list;
    op_inserter_t op_inserter(op_list);

    size_t dist = editdist<op_inserter_t>(s1.data(), s2.data(), s1.size(), s2.size(), (void*)buffer, 4, &op_inserter);
    std::cout << dist;// << std::endl;
    for (size_t i = 0; i < op_list.size(); ++i) {
      std::cout << " [" << op_list[i].op_ << "," << op_list[i].value_ << "]";
    }
    std::cout << std::endl;
   
    print_acceptor_t printer(std::cout, s1,s2);
    calc_edit_substrings(op_list, printer);
    std::cout << std::endl;

    std::cout << count_edit_substrings(op_list) << std::endl; 

    edit_count_t edit_count;
    calc_edit_count(op_list, edit_count);
    std::cout << edit_count << std::endl;
  }


  return 0;
}
