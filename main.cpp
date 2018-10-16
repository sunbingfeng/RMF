//Copyright (c) 2011 ashelly.myopenid.com under <http://w...content-available-to-author-only...e.org/licenses/mit-license>

#include <stdlib.h>
#include <algorithm>
#include <cassert>
#include <chrono>
#include <deque>
#include <exception>
#include <iostream>
#include <iterator>
#include <limits>
#include <list>
#include <memory>
#include <random>
#include <sstream>
#include "rolling_median_filter.hpp"

using namespace std;

// Customize for your data Item type
typedef int Item;



/**
 * @brief      test app
 *
 * @param[in]  argc  The argc
 * @param      argv  The argv
 * @example: ./main 100 1000
 *           100 means the rolling window size, and 1000 means the total iteration count.
 *
 * @return     { description_of_the_return_value }
 */
int main(int argc, char* argv[])
{
   int i;
   Item v;
   int win_size = atoi(argv[1]);
   auto m = std::make_shared<Mediator<Item> >(win_size);

   std::vector<Item> input;
   std::vector<double> timing;

   int it = atoi(argv[2]);

   for (i=0;i<it;i++)
   {
      v = 0.001*(rand()&65535);
      input.push_back(v);
      if(input.size() > win_size)
      {
         input.erase(input.begin());
      }

      auto start = std::chrono::high_resolution_clock::now();
      m->insert(v);
      std::cout << "median: " << m->median() << std::endl;
      auto end = std::chrono::high_resolution_clock::now();
      double dur = std::chrono::duration<double>(end - start).count();
      timing.push_back(dur);
   }

   std::sort(input.begin(), input.end(), std::greater<Item>());
   std::cout << "sorted list of latest input: ";
   for (std::vector<Item>::iterator i = input.begin(); i != input.end(); ++i)
   {
      std::cout << *i << " ";
   }
   std::cout << std::endl;

   double sum = 0.0;
   for (std::vector<double>::iterator i = timing.begin(); i != timing.end(); ++i)
   {
      sum += *i;
   }
   sum /= timing.size();
   std::cout << "average iterator cost: " << sum << std::endl;
}