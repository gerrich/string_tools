#pragma once

#include <string.h>

inline int min(int a, int b) {
  return (a<b ? a : b);
}
inline int min(int a, int b, int c) {
  return (a<b ? (a<c ? a : c) : (b<c ? b : c));
}

// T = O(l1 * l2)
int editdist(const char *s, const char *t,int l1,int l2) 
{
  int d1,d2,d3,cost;
  int i,j;
  int d[256][256];
  for(i=0; i<=l1; i++) 
  {
    for(j=0;j<=l2;j++)
    {
      if(s[i+1]==t[j+1]) 
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

struct _buffer_t {
  _buffer_t(int *data, size_t max_dist): data_(data), max_dist_(max_dist) {}
  int& operator() (size_t i, size_t j) {
    return data_[max_dist_ * 2 * j  + i];
  }
  
  int *data_;
  size_t max_dist_;
};
// external buffer, max_dist O((l1 + l2) * max_dist)
int editdist(const char *str, const char *tpl, size_t str_len, size_t tpl_len, int *dist_buffer, size_t max_dist) 
{
  _buffer_t buffer(dist_buffer, max_dist + 1);
  for (size_t i=0; i <= max_dist; ++i) {
    buffer(0, i) = i;
    buffer(i, 0) = i;
  }
  int d1,d2,d3,cost;
  int i,j;

  for(i=0; i <= str_len; ++i) 
  {
    j = i > max_dist ? i - max_dist : 0;
    for(; j <= tpl_len && j <= i + max_dist; ++j)
    {
      if(str[i+1]==tpl[j+1]) 
        cost=0;
      else
        cost=1;

      if (i + max_dist > j)
        d1 = buffer(i, j+1) + 1;
      else
        d1 = max_dist;

      if (j + max_dist > i)
        d2 = buffer(i+1, j) + 1;
      else
        d2 = max_dist;

      d3 = buffer(i, j) + cost;

      buffer(i+1, j+1) = min(d1,d2,d3);
      if(i>0 && j>0 && str[i]==tpl[j-1] && str[i-1]==tpl[j] )   //transposition
      {
        buffer(i+1, j+1)=min(buffer(i+1,j+1), buffer(i-1, j-1) + cost);
      }
    }
  }
  return buffer(str_len + 1, tpl_len + 1); 
}
