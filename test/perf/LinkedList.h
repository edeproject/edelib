#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__

template <typename T>
class LinkedList {
	public:
		struct Node {
			T value;

			Node* next;
			Node* prev;
		};

	private:
		unsigned int sz;
		Node* last;
		Node* start;

	public:
		LinkedList() : sz(0), last(0), start(0) { }
		~LinkedList() { clear(); }

		unsigned int size(void) { return sz; }

		void clear(void) {
			sz = 0;

			Node* next;
			for(; start; start = next) {
				next = start->next;
				delete start;
			}
		}

		void push_back(const T& val) {
			Node* n = new Node;
			n->value = val;
			n->next = n->prev = 0;

			if(!start)
				start = n;

			n->prev = last;

			if(last)
				last->next = n;

			last = n;

			sz++;
		}

		void push_front(const T& val) {
			Node* n = new Node;
			n->value = val;
			n->next = n->prev = 0;

			if(!start) {
				start = n;
				last = n;
			} else {
				n->next = start;
				start->prev = n;
				start = n;
			}

			sz++;
		}

		Node* begin(void) const { return start; }
};

#endif
