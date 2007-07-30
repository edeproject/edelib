/*
 * $Id$
 *
 * Part of edelib.
 * Copyright (c) 2005-2007 EDE Authors.
 *
 * This program is licenced under terms of the 
 * GNU General Public Licence version 2 or newer.
 * See COPYING for details.
 */

#ifndef __LIST_H__
#define __LIST_H__

#include "econfig.h"
#include "Debug.h"

EDELIB_NS_BEGIN

/**
 * \class List
 * \brief Linked list class
 *
 * This implementation is very similar to std::list, providing subset of the 
 * same methods with the same behaviours. On other hand, it does not try
 * to implement all methods from std::list, since main goal is to keep it
 * small and simple, which will as result generate much smaller code.
 *
 * Also, size() method differ from std::list; some implementations run it
 * in linear time, but some in constant time. This implementation returns size()
 * result in constant time.
 *
 * Using list is the same as for std::list; all traversal is done via iterators.
 * Here is simple sample:
 * \code
 *   List<int> ls;
 *   ls.push_back(34);
 *   ls.push_back(4);
 *   ls.push_front(34);
 *   List<int>::iterator it = ls.begin();
 *   while(it != ls.end()) 
 *     printf("%i\n", (*it));
 * \endcode
 *
 * Note that working with list iterators, great care must be taken where and when to retrieve
 * iterators for start and end of the list. For example, this code is <b>invalid</b>:
 * \code
 *   List<int> ls;
 *
 *   // BAD, pointer to first element will be changed if list is empty
 *   List<int>::iterator it = ls.begin();
 *   ls.push_back(34);
 *
 *   // or...
 *   List<int>::iterator it = ls.begin();
 *   // push_front() will invalidate previously retrieved iterator
 *   ls.push_front(34);
 *
 *   // or...
 *   ls.push_front(34);
 *   List<int>::iterator it = ls.begin();
 *   // insert at the start invalidated begin()
 *   ls.insert(it, 33);
 * \endcode
 *
 * Correct way is retrieve iterator <b>just before</b> iterator will be used, like:
 * \code
 *   List<int> ls;
 *   ls.push_back(34);
 *   ls.push_back(4);
 *   ls.push_front(4);
 *   List<int>::iterator it = ls.begin();
 *   // rest...
 * \endcode
 *
 * This issue is not related to this list implementation only; libstdc++ list will
 * return garbage in above cases (but in some will crash); this implementation will
 * always crash when above cases occurs, so be carefull :-P.
 */
template <typename T>
class List {
	private:
		struct ListNode {
			ListNode(const T& v) : value(v), next(0), prev(0) { }

			T value;
			ListNode* next;
			ListNode* prev;
		};
	
		ListNode* start;
		ListNode* last;
		unsigned int sz;

	public:
#ifndef SKIP_DOCS
		class iterator {
			private:
				ListNode* curr;
				iterator(ListNode* start) : curr(start) { }
				friend class List<T>;

			public:
				iterator() : curr(0) { }

				T& operator*(void) { 
					EASSERT(curr != 0 && "Bad code! Dereferencing NULL iterator !!!"); 
					return curr->value; 
				}

				iterator& operator++(void) { curr = curr->next; return *this; }
				iterator& operator--(void) { curr = curr->prev; return *this; }
				bool operator!=(const iterator& i) { return curr != i.curr; }
				bool operator==(const iterator& i) { return curr == i.curr; }
				operator bool(void) const { return !(curr == 0); }
		};
#endif

		/**
		 * Creates an empty list
		 */
		List() : start(0), last(0), sz(0) { }

		/**
		 * Clears data
		 */
		~List() { clear(); }

		/**
		 * Clear all data
		 */
		void clear(void) {
			if(!sz)
				return;

			ListNode* next;
			for(; start; start = next) {
				next = start->next;
				delete start;
			}

			sz = 0;
			start = last = 0;
		}

		/**
		 * Remove element given at iterator position.
		 *
		 * \return iterator pointing to the next element
		 * \param it is element to be removed
		 */
		iterator erase(iterator& it) {
			if(sz <= 1) {
				clear();
				return iterator();
			}

			if(it.curr == start) {
				start = it.curr->next;
				start->prev = 0;
			} else if(it.curr == last) {
				last = it.curr->prev;
				last->next = 0;
			} else {
				EASSERT(it.curr->prev);
				EASSERT(it.curr->next);
				it.curr->prev->next = it.curr->next;
				it.curr->next->prev = it.curr->prev;
			}

			iterator ret(it.curr);
			++ret;
			sz--;
			delete it.curr;
			return ret;
		}

		/**
		 * Inserts given value at position <b>before</b> position poiniting 
		 * by given iterator.
		 *
		 * \return iterator pointing to inserted element
		 * \param it is location before to be inserted
		 * \param val is value to insert
		 */
		iterator insert(iterator& it, const T& val) {
			if(it.curr == 0 || it.curr == last) {
				// push_back() increase sz
				push_back(val);
				return iterator(last);
			} else if(it.curr == start) {
				// push_front() increase sz
				push_front(val);
				return iterator(start);
			} else {
				ListNode* n = new ListNode(val);
				n->next = it.curr;
				n->prev = it.curr->prev;
				it.curr->prev->next = n;
				sz++;
				return iterator(n);
			}
		}

		/**
		 * Adds new value to the end of the list.
		 * \param val is value to be added
		 */
		void push_back(const T& val) {
			sz++;
			ListNode* n = new ListNode(val);

			if(!start) {
				start = n;
				last = start;
			} else {
				n->prev = last;
				last->next = n;
				last = n;
			}
		}

		/**
		 * Adds new value to the beginning of the list.
		 * \param val is value to be added
		 */
		void push_front(const T& val) {
			sz++;
			ListNode* n = new ListNode(val);

			if(!start) {
				start = n;
				last = start;
			} else {
				start->prev = n;
				n->next = start;
				n->prev = 0;
				start = n;
			}
		}

		/**
		 * Return iterator pointing to the start of the list.
		 */
		iterator begin(void) const { return iterator(start); }

		/**
		 * Return iterator pointing <b>after</b> the end of the list.
		 * <b>Do not</b> dereference that iterator requesting value
		 * of latest element (this is required by Standard). To get last
		 * element, decrement iterator value, like:
		 * \code
		 *   List<int> ls;
		 *   ...
		 *   List<int>::iterator it = ls.end();
		 *   // printf("%i\n", (*it));   // BAD
		 *
		 *   --it;                   // correct
		 *   printf("%i\n", (*it));  // correct
		 * \endcode
		 */
		iterator end(void) const { return iterator(last ? last->next : last); }

		/**
		 * Return size of list.
		 */
		unsigned int size(void) const { return sz; }

		/**
		 * Return true if list is empty; otherwise false.
		 */
		bool empty(void) const { return size() == 0; }
};

#define list List

EDELIB_NS_END
#endif // __LIST_H__
