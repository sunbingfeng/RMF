//Copyright (c) 2011 ashelly.myopenid.com under <http://w...content-available-to-author-only...e.org/licenses/mit-license>

#ifndef ROLLING_MEDIAN_FILTER_H
#define ROLLING_MEDIAN_FILTER_H

#include <memory>
#include <sstream>

template <class T>
class Mediator
{
public:
   Mediator(int nItems);
   ~Mediator();

   int mmless(int i, int j);
   int mmexchange(int i, int j);
   int mmCmpExch(int i, int j);
   void minSortDown(int i);
   void maxSortDown(int i);
   int minSortUp(int i);
   int maxSortUp(int i);

   int minCt();
   int maxCt();

   T ItemLess(T a, T b) { return ((a)<(b)); };
   T ItemMean(T a, T b) { return (((a)+(b))/2); };

   void insert(T v);
   T median();
private:
   T* data;  //circular queue of values
   int*  pos;   //index into `heap` for each value
   int*  heap;  //max/median/min heap holding indexes into `data`.
   int   N;     //allocated size.
   int   idx;   //position in circular queue
   int   ct;    //count of items in queue   
};

/*--- Helper Functions ---*/
template <class T> int Mediator<T>::minCt()
{
   return (ct-1)/2; //count of items in minheap
}

template <class T> int Mediator<T>::maxCt()
{
   return (ct)/2;   //count of items in maxheap 
}

//returns 1 if heap[i] < heap[j]
template <class T> int Mediator<T>::mmless(int i, int j)
{
   return ItemLess(data[heap[i]],data[heap[j]]);
}

//swaps items i&j in heap, maintains indexes
template <class T> int Mediator<T>::mmexchange(int i, int j)
{
   int t = heap[i];
   heap[i]=heap[j];
   heap[j]=t;
   pos[heap[i]]=i;
   pos[heap[j]]=j;
   return 1;
}

//swaps items i&j if i<j;  returns true if swapped
template <class T> int Mediator<T>::mmCmpExch(int i, int j)
{
   return (mmless(i,j) && mmexchange(i,j));
}

//maintains minheap property for all items below i/2.
template <class T> void Mediator<T>::minSortDown(int i)
{
   for (; i <= minCt(); i*=2)
   {  if (i>1 && i < minCt() && mmless(i+1, i)) { ++i; }
      if (!mmCmpExch(i,i/2)) { break; }
   }
}

//maintains maxheap property for all items below i/2. (negative indexes)
template <class T> void Mediator<T>::maxSortDown(int i)
{
   for (; i >= -maxCt(); i*=2)
   {  if (i<-1 && i > -maxCt() && mmless(i, i-1)) { --i; }
      if (!mmCmpExch(i/2,i)) { break; }
   }
}

//maintains minheap property for all items above i, including median
//returns true if median changed
template <class T> int Mediator<T>::minSortUp(int i)
{
   while (i>0 && mmCmpExch(i,i/2)) i/=2;
   return (i==0);
}

//maintains maxheap property for all items above i, including median
//returns true if median changed
template <class T> int Mediator<T>::maxSortUp(int i)
{
   while (i<0 && mmCmpExch(i/2,i))  i/=2;
   return (i==0);
}

//creates new Mediator: to calculate `nItems` running median. 
//mallocs single block of memory, caller must free.
template <class T> Mediator<T>::Mediator(int nItems)
{
   int size = nItems*(sizeof(T)+sizeof(int)*2);

   data= (T*)malloc(size);
   pos = (int*) (data+nItems);
   heap = pos+nItems + (nItems/2); //points to middle of storage.
   N=nItems;
   ct = idx = 0;
   while (nItems--)  //set up initial heap fill pattern: median,max,min,max,...
   {  pos[nItems]= ((nItems+1)/2) * ((nItems&1)?-1:1);
      heap[pos[nItems]]=nItems;
   }
}

template <class T> Mediator<T>::~Mediator()
{
   free(data);
}

//Inserts ite maintains median in O(lg nItems)
template <class T> void Mediator<T>::insert(T v)
{
   int isNew=(ct<N);
   int p = pos[idx];
   T old = data[idx];
   data[idx]=v;
   idx = (idx+1) % N;
   ct+=isNew;
   if (p>0)         //new item is in minHeap
   {  if (!isNew && ItemLess(old,v)) { minSortDown(p*2);  }
      else if (minSortUp(p)) { maxSortDown(-1); }
   }
   else if (p<0)   //new item is in maxheap
   {  if (!isNew && ItemLess(v,old)) { maxSortDown(p*2); }
      else if (maxSortUp(p)) { minSortDown(1); }
   }
   else            //new item is at median
   {  if (maxCt()) { maxSortDown(-1); }
      if (minCt()) { minSortDown(1); }
   }
}

//returns median item (or average of 2 when item count is even)
template <class T> T Mediator<T>::median()
{
   T v= data[heap[0]];
   if ((ct&1)==0) { v= ItemMean(v,data[heap[-1]]); }
   return v;
}

#endif