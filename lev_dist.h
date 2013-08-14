#pragma once

#include <string.h>
#include <stdint.h>
#include <stack>
#include <cassert>
#include <vector> 

template <typename T>
inline const T& min(const T &a, const T &b) {
  return (less(a, b) ? a : b);
}
template <typename T>
inline const T& min(const T &a, const T &b, const T&c) {
  return (less(a, b) ? (less(a, c) ? a : c) : (less(b, c) ? b : c));
}
/*
// T = O(l1 * l2)
int editdist(const char *s, const char *t,int l1,int l2) 
{
  int d1,d2,d3,cost;
  int i,j;
  int d[256][256];
  for(i=0; i<l1; ++i) 
  {
    for(j=0; j<l2; ++j)
    {
      if(s[i]==t[j]) 
        cost=0;
      else
        cost=1;
      d1=d[i][j+1]+1;
      d2=d[i+1][j]+1;
      d3=d[i][j]+cost;
      d[i+1][j+1]=min(d1,d2,d3);
      if(i>0 && j>0 && s[i]==t[j-1] && s[i-1]==t[j] )   //transposition
      {
        d[i+1][j+1]=min(d[i+1][j+1],d[i-1][j-1]+cost);
      }
    }
  }
  return d[l1+1][l2+1]; 
}
*/

template <typename data_t>
struct _buffer_t {
  _buffer_t(data_t *data, size_t max_dist): data_(data), max_dist_(max_dist) {}
  
  data_t& operator() (size_t i, size_t j) {
    return data_[max_dist_ * 2 * j  + i];
  }
  const data_t& operator() (size_t i, size_t j) const {
    return data_[max_dist_ * 2 * j  + i];
  }
  
  data_t *data_;
  size_t max_dist_;
};

struct _lev_state_t{
  uint32_t op_:2;
  uint32_t value_:30;
};

_lev_state_t& operator +=(_lev_state_t &lhs, const uint32_t &rhs) {
  lhs.value_ += rhs;
  return lhs;
}
_lev_state_t operator +(const _lev_state_t& lhs, const uint32_t &rhs) {
  _lev_state_t data = lhs;
  return (data += rhs);
}

const uint32_t op_paste = 0;
const uint32_t op_replace = 0;
const uint32_t op_delete = 1;
const uint32_t op_insert = 2;
const uint32_t op_transpose = 3;

inline bool less(const _lev_state_t &lhs, const _lev_state_t &rhs) {
  return lhs.value_ < rhs.value_;
}

struct op_inserter_t {
  op_inserter_t(std::vector<_lev_state_t> &data) : data_(data) {}
  inline void insert(const _lev_state_t &op) {
    data_.push_back(op);
  }
  std::vector<_lev_state_t> &data_;
};
// external buffer, max_dist O((l1 + l2) * max_dist)
template <typename op_out_t>
int editdist(
    const char *str,
    const char *tpl,
    size_t str_len,
    size_t tpl_len,
    void *dist_buffer,
    size_t max_dist,
    op_out_t *op_out_ptr = NULL) 
{
  typedef _lev_state_t op_t;

  if (str_len > tpl_len + max_dist or str_len + max_dist < tpl_len) return max_dist;

  _buffer_t<op_t> buffer((op_t*)dist_buffer, max_dist + 1);

  buffer(0,0) = (op_t){op_paste, 0};
  for (size_t i=1; i <= max_dist; ++i) {
    buffer(0, i) = (op_t){op_insert, i};
    buffer(i, 0) = (op_t){op_delete, i};
  }

  op_t d0, d1, d2;
  uint32_t cost;

  for(size_t i=0; i < str_len; ++i) 
  {
    size_t j = i > max_dist ? i - max_dist : 0;
    for(; j < tpl_len && j <= i + max_dist; ++j)
    {
      if(str[i]==tpl[j]) { 
        cost=0;
      } else {
        cost=1;
      }
      d0 = buffer(i, j) + cost;
      d0.op_ = op_paste;

      if (i + max_dist > j) {
        d1 = buffer(i, j+1) + 1;
        d1.op_ = op_delete;
      } else {
        d1 = d0;
      }

      if (j + max_dist > i) {
        d2 = buffer(i+1, j) + 1;
        d2.op_ = op_insert;
      } else {
        d2 = d0;
      }


      buffer(i+1, j+1) = min<op_t>(d0, d1, d2);
      if (i>0 && j>0 && str[i] == tpl[j-1] && str[i-1] == tpl[j] ) {  //transposition
        if ( less(buffer(i-1, j-1) + cost, buffer(i+1,j+1)) ) {
          buffer(i+1, j+1) = buffer(i-1, j-1) + cost;
          buffer(i+1, j+1).op_ = op_transpose; 
        }
      }
    }
  }

  // restore change path
  if (op_out_ptr) {
    std::stack<op_t> op_stack;
    for (int i = str_len, j = tpl_len; i > 0 or j > 0;) {
      switch (buffer(i,j).op_) {
        case 0: //paste // replace
          --i;
          --j;
          if (str[i] == tpl[j]) { // paste
          //  op_stack.push((op_t){op_paste,i});
          } else {
            op_stack.push((op_t){op_replace,i});
          }
          break;
        case 1: //delete
          --i;
          op_stack.push((op_t){op_delete,i});
          break;
        case 2: //insert
          --j;
          op_stack.push((op_t){op_insert,i});
          break;
        case 3: // transposition
          i -= 2;
          j -= 2;
          op_stack.push((op_t){op_transpose,i});
          break;
        default:
          assert(false);
      }
    }
    while (!op_stack.empty()) {
      op_t &op = op_stack.top();
      op_out_ptr->insert(op);
      op_stack.pop();
    }
  }
  
  return buffer(str_len, tpl_len).value_; 
}
