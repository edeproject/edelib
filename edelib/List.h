/*
 * $Id$
 *
 * A simple List class.
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

#ifndef SKIP_DOCS
template <typename T>
struct ListNode {
	T* value;
	ListNode* next;
	ListNode* prev;

	// for dummy node
	ListNode() : value(0), next(0), prev(0) { }

	// for real node
	ListNode(const T& v) : value(new T(v)), next(0), prev(0) { }

	~ListNode() { 
		//EDEBUG("ListNode::~ListNode\n");
		delete value; 
		value = 0; 
		next = prev = 0; 
	}
};

template <typename T>
struct ListIterator {
	typedef ListNode<T> NodeType;

	NodeType* node;

	ListIterator(NodeType* n) : node(n) { }
	ListIterator() : node(0) { }

	T& operator*(void) const { 
		EASSERT(node != 0 && "Bad code! Access to zero node!!!"); 
		EASSERT(node->value != 0 && "Bad code! Dereferencing NULL value!!!"); 
		return *(node->value); 
	}

	bool operator!=(const ListIterator& other) const { return node != other.node; }
	bool operator==(const ListIterator& other) const { return node == other.node; }
	ListIterator& operator++(void) { node = node->next; return *this; }
	ListIterator& operator--(void) { node = node->prev; return *this; }
};
#endif

/**
 * \class list
 * \brief Linked list class
 *
 * This implementation is very similar to std::list, providing subset of the 
 * same methods with the same behaviours. On other hand, it does not try
 * to implement all methods from std::list, since main goal is to keep it
 * small and simple, which will as result generate much smaller code.
 *
 * Also, size() method differs from std::list; some implementations run it
 * in linear time, but some in constant time. This implementation returns size()
 * result always in constant time.
 *
 * Using list is the same as for std::list; all traversal is done via iterators.
 * Here is sample:
 * \code
 *   list<int> ls;
 *   ls.push_back(34);
 *   ls.push_back(4);
 *   ls.push_front(34);
 *   list<int>::iterator it = ls.begin();
 *
 *   while(it != ls.end()) {
 *     printf("%i\n", (*it));
 *     ++it;
 *   }
 * \endcode
 *
 * Note that working with list iterators, great care must be taken where and when to retrieve
 * iterators for start and end of the list. For example, this code is <b>invalid</b>:
 * \code
 *   list<int> ls;
 *
 *   // BAD, pointer to first element will be changed if list is empty
 *   list<int>::iterator it = ls.begin();
 *   ls.push_back(34);
 *
 *   // or...
 *   list<int>::iterator it = ls.begin();
 *   // push_front() will invalidate previously retrieved iterator
 *   ls.push_front(34);
 *
 *   // or...
 *   ls.push_front(34);
 *   list<int>::iterator it = ls.begin();
 *   // insert at the start invalidated begin()
 *   ls.insert(it, 33);
 * \endcode
 *
 * Correct way is retrieve iterator <b>just before</b> iterator will be used, like:
 * \code
 *   list<int> ls;
 *   ls.push_back(34);
 *   ls.push_back(4);
 *   ls.push_front(4);
 *   list<int>::iterator it = ls.begin();
 *   // rest...
 * \endcode
 *
 * This issue is not related to this list implementation only; libstdc++ list will
 * return garbage in above cases (but in some will crash); this implementation will
 * always crash when above cases occurs, so be carefull :-P.
 */
template <typename T>
class list {
	private:
		typedef ListNode<T> Node;
		typedef unsigned int size_type;
		typedef bool (*SortCmp)(const T& val1, const T& val2);

		size_type sz;
		Node* tail;

		list(const list&);
		list& operator=(const list&);

		static bool default_sort_cmp(const T& val1, const T& val2) { return val1 < val2; }

	public:
		/*
		 * This comment is not part of documentation, and in short explains
		 * implementation details.
		 *
		 * List is implemented as circular doubly linked list, which means
		 * that last node is pointing back to the first one (and reverse). 
		 * Due the nature of traversing in circular lists, additional node 
		 * (dummy node) is created so traversal (first != last) can be done.
		 *
		 * As you can see, only one node (tail) is used; it always pointing
		 * to dummy node (which is always latest node). To get first element node, 
		 * tail->first is used, and to get last (user visible), tail->prev is used. 
		 * This implementation is needed so cases like --end() can return valid 
		 * iterator to last element in the list.
		 *
		 * I tried to avoid dummy node creation, but implementing circular list
		 * will be pain in the ass. Also, contrary popular std::list implementations I could
		 * find, this node will be created only when insert()/push_back()/push_front()
		 * are called; without those calls, list will not allocate it.
		 */
#ifndef SKIP_DOCS
		typedef ListIterator<T> iterator;
#endif

		/**
		 * Creates an empty list
		 */
		list() : sz(0), tail(0) { }

		/**
		 * Clears data
		 */
		~list() { clear(); } 

		/**
		 * Clear all data
		 */
		void clear(void) { 
			if(!tail) {
				EASSERT(sz == 0 && "Internal error! size() != 0, but list is empty !?!!");
				return;
			}

			Node* p = tail->next;
			Node* t;
			while(p != tail) {
				t = p->next;
				delete p;
				p = t;
			}

			// delete dummy node
			delete tail;

			tail = 0;
			sz = 0;
		}

		/**
		 * Inserts given value at position <b>before</b> position poiniting 
		 * by given iterator.
		 *
		 * \return iterator pointing to inserted element
		 * \param it is location before to be inserted
		 * \param val is value to insert
		 */
		iterator insert(iterator it, const T& val) {
			// [23.2.2.3] insert() does not affect validity of iterators
			Node* tmp = new Node(val);
			if(!tail) {
				// dummy node first
				tail = new Node;
				tail->next = tail->prev = tmp;
				tmp->next = tmp->prev = tail;
			} else {
				tmp->next = it.node;
				tmp->prev = it.node->prev;
				it.node->prev->next = tmp;
				it.node->prev = tmp;
			}

			sz++;
			return iterator(tmp);
		}

		/**
		 * Remove element given at iterator position.
		 *
		 * \return iterator pointing to the next element
		 * \param it is element to be removed
		 */
		iterator erase(iterator it) {
			if(sz <= 1) {
				clear();
				return 0;
			}

			// do not allow erase(l.end())
			EASSERT(it.node != tail && "Bad code! erase() on end()!!!");

			// [23.2.2.3] erase() invalidates only the iterators
			it.node->prev->next = it.node->next;
			it.node->next->prev = it.node->prev;

			iterator ret(it.node);
			++ret;
			sz--;
			delete it.node;
			return ret;
		}

		/**
		 * Adds new value to the end of the list.
		 * \param val is value to be added
		 */
		void push_back(const T& val) { insert(end(), val); }

		/**
		 * Adds new value to the beginning of the list.
		 * \param val is value to be added
		 */
		void push_front(const T& val) { insert(begin(), val); }

		/**
		 * Return iterator pointing to the start of the list.
		 */
		iterator begin(void) const { return (tail ? tail->next : 0); }

		/**
		 * Return iterator pointing <b>after</b> the end of the list.
		 * <b>Do not</b> dereference that iterator requesting value
		 * of latest element. 
		 */
		iterator end(void) const { return tail; }

		/**
		 * Return reference to first element in the list.
		 */
		T& front(void) { return *(begin()); }

		/**
		 * Return const reference to first element in the list.
		 */
		const T& front(void) const { return *(begin()); }

		/**
		 * Return reference to last element in the list.
		 */
		T& back(void) { return *(--end()); }

		/**
		 * Return const reference to last element in the list.
		 */
		const T& back(void) const { return *(--end()); }

		/**
		 * Return size of list.
		 */
		size_type size(void) const { return sz; }

		/**
		 * Return true if list is empty; otherwise false.
		 */
		bool empty(void) const { return sz == 0; }

#ifndef SKIP_DOCS
		void __do_merge(iterator first1, iterator last1, iterator last2, SortCmp cmp) {
			size_type sz = 0;
			for(iterator i = first1; i != last1; sz++, ++i) 
				;

			for(iterator first2 = last1; sz > 0 && first2 != last2; ) {
				if(cmp(*first2, *first1)) {
					iterator it_next = first2;
					++it_next;

					/* transfer */
					it_next.node->prev->next = first1.node;
					first2.node->prev->next = it_next.node;
					first1.node->prev->next = first2.node;

					Node* tmp = first1.node->prev;

					first1.node->prev = it_next.node->prev;
					it_next.node->prev = first2.node->prev;
					first2.node->prev = tmp;
					/* transfer ends */

					first2 = it_next;
				} else {
					++first1;
					--sz;
				}
			}
		}

		/* combsort11 implementation */
		void comb_sort(SortCmp cmp = 0) {
			if(size() <= 1)
				return;

			if(!cmp)
				cmp = default_sort_cmp;

			unsigned int gap = size();
			bool swapped;

			iterator it;
			do {
				swapped = false;
				/* shrink factor */
				gap = (unsigned int)(gap / 1.3);
				if(gap == 9 || gap == 10)
					gap = 11;
				else if(gap < 1)
					gap = 1;

				it = begin();
				for(unsigned int i = 0; i < size() - gap; i++, ++it) {
					iterator tmp = it;

					for(unsigned int j = 0; j < gap; j++)
						++tmp;

					if(cmp(*tmp, *it)) {
						/* exchange values not nodes */
						T* val = tmp.node->value;
						tmp.node->value = it.node->value;
						it.node->value = val;
					}
				}
			} while(gap > 1 || swapped);
		}
#endif

		/**
		 * Sorts list. If cmp function is given (in form <em>bool cmp(const T& v1, const T& v2)</em>,
		 * elements will be compared with it.
		 */
		void sort(SortCmp cmp = 0) {
			if(size() <= 1)
				return;

			if(!cmp)
				cmp = default_sort_cmp;

			iterator it1, it2, it3;

			for(size_type i = 1; i < size(); i *= 2) {
				it1 = begin();
				it2 = begin();
				it3 = begin();

				for(size_type j = i; j > 0 && it2 != end(); j--)
					++it2;

				for(size_type j = i * 2; j > 0 && it3 != end(); j--)
					++it3;

				size_type msz = size() + (2 * i) / (i * 2);

				for(size_type m = 0; m < msz; m++) {
					if(it1 != end() && it2 != end()) {
						/* merge */
						__do_merge(it1, it2, it3, cmp);
						
						it1 = it3;
						it2 = it3;

						for(size_type j = i; j > 0 && it2 != end(); j--)
							++it2;
						for(size_type j = i * 2; j > 0 && it3 != end(); j--)
							++it3;
					}
				}
			}
		}
};

EDELIB_NS_END

#endif
