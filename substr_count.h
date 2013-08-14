#pragma once
#include "lev_dist.h"

#include <iostream>
#include <string>
#include <vector>

struct edit_count_t {
  edit_count_t() 
    : n_delete(0)
    , n_insert(0)
    , n_replace(0)
    , n_transpose(0) {}
  
  void clear() {
    edit_count_t tmp;
    std::swap(*this, tmp);
  }

  int n_delete;
  int n_insert;
  int n_replace;
  int n_transpose;
};

std::ostream& operator << (std::ostream& os, const edit_count_t &d) {
  os
    << "[d:" << d.n_delete    << ", "
    << "i:"  << d.n_insert    << ", "
    << "r:"  << d.n_replace   << ", "
    << "t:"  << d.n_transpose << "]";
}

void calc_edit_count(const std::vector<_lev_state_t> &op_list, edit_count_t &data) { 
  data.clear();
  for (size_t i = 0; i < op_list.size(); ++i) {
    switch (op_list[i].op_) {
      case op_paste:
        ++data.n_replace;
        break;
      case op_delete:
        ++data.n_delete;
        break;
      case op_insert:
        ++data.n_insert;
        break;
      case op_transpose:
        ++data.n_transpose;
        break;
    };
  }
}

template <typename acceptor_t>
size_t calc_edit_substrings(/*const std::string &s1, const std::string &s2, */const std::vector<_lev_state_t> &op_list, acceptor_t &acceptor) {
  size_t substr_count = 0;
  size_t s_i = 0;
  size_t t_i = 0;
  size_t ns_i = 0;
  size_t nt_i = 0;
  
  for (size_t i = 0; i < op_list.size(); ++i) {
    size_t d_i = op_list[i].value_ - ns_i;
    if (d_i) {
      if (ns_i - s_i > 0 or nt_i - t_i > 0) {
        acceptor(s_i, ns_i-s_i, t_i, nt_i-t_i);
      }
      acceptor(ns_i, d_i);
      //assert(s1.substr(ns_i,d_i) == s2.substr(nt_i,d_i));
      //std::cout << "("<< s1.substr(ns_i,d_i) <<")";
      ns_i+=d_i;
      nt_i+=d_i;
      s_i = ns_i;
      t_i = nt_i;
    }

    switch (op_list[i].op_) {
      case op_paste:
        ns_i += 1;
        nt_i += 1;
        break;
      case op_delete:
        ns_i += 1;
        nt_i += 0;
        break;
      case op_insert:
        ns_i += 0;
        nt_i += 1;
        break;
      case op_transpose:
        ns_i += 2;
        nt_i += 2;
        break;
    };
  }
  if (ns_i - s_i > 0 or nt_i - t_i > 0) {
    acceptor(s_i, ns_i-s_i, t_i, nt_i-t_i);
    //std::cout << "{"<< s1.substr(s_i,ns_i-s_i) <<"|"<< s2.substr(t_i,nt_i-t_i) << "}";
  }
  
  acceptor(ns_i, std::string::npos);
  return substr_count;
}

struct count_acceptor_t {
  count_acceptor_t(): counter_(0) {}

  //void operator() (size_t pos_1, size_t len_1, size_t pos_2, size_t pos2) const {
  void operator() (size_t, size_t, size_t, size_t) {
    ++counter_;
  }
  void operator() (size_t, size_t) {
//    ++counter;
  }

  operator size_t() {
    return counter_;
  }

  size_t counter_;
};


size_t count_edit_substrings(/*const std::string &s1, const std::string &s2, */const std::vector<_lev_state_t> &op_list) {
  count_acceptor_t acceptor;
  calc_edit_substrings(/*s1, s2, */op_list, acceptor);
  return acceptor;
}

struct print_acceptor_t {
  print_acceptor_t(std::ostream &os, const std::string &s1, const std::string &s2)
    : os_(os)
    , s1_(s1)
    , s2_(s2) {}


  void operator() (size_t pos_1, size_t len_1, size_t pos_2, size_t len_2) const {
    os_ << "{"  << s1_.substr(pos_1, len_1) << "|" << s2_.substr(pos_2, len_2) << "}"; 
  }
  void operator() (size_t pos_1, size_t len_1) const {
    if (pos_1 < s1_.size())
    os_ << "(" << s1_.substr(pos_1, len_1) << ")";
  }

  mutable std::ostream &os_;
  const std::string &s1_;
  const std::string &s2_;
};


