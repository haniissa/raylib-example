#include <fmt/format.h>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <fmt/base.h>
#include <fmt/core.h>
#include <fmt/ranges.h>



int main(){
  std::vector<int> count{1,2,3,4,5,6,7,8,9,10};
  std::vector<int>  result;

  auto out = std::ostream_iterator<int>(std::cout, ", ");
  auto is_range = [](int val){val >= 5 and val <= 8;}
  std::copy_if(std::begin(count), std::end(count), std::begin(result), is_range);
}
