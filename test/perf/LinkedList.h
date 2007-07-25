#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__

#ifndef NULL
	#define NULL 0
#endif

template <typename T>
class LinkedList {
	public:
		struct Data {
			T* value;
			Data* head;
			Data* next;
			Data* prev;
		};
	private:
		Data* data;
		Data* curr;
		int sz;

	public:
		LinkedList() : data(NULL), curr(NULL), sz(0) { }
		~LinkedList() { clear(); }

		void clear(void) {
			if(!data)
				return;

			Data* next;
			for(data = data->head; data; data = next) {
				next = data->next;
				//printf("Clearing item\n");
				if(data->value)
					delete data->value;

				delete data;
				data = NULL;
			}

			sz = 0;
		}

		void push_back(const T& t) {
			Data* tmp = new Data;
			tmp->next = NULL;
			tmp->value = new T(t);

			if(!data) {
				data = tmp;
				data->head = data;
				data->prev = data;
			} else {
				tmp->prev = data;
				tmp->head = data->head;
				data->next = tmp;
				data = tmp;
			}

			// at start, let current() point to head
			if(!curr)
				curr = data->head;

			sz++;
		}

		void push_front(const T& t) {
			Data* tmp = new Data;
			tmp->value = new T(t);

			if(!data) {
				data = tmp;
				data->head = data;
				data->prev = data;
				data->next = NULL;
			} else {
				// exchange values only :-P
				T* val = tmp->value;
				tmp->value = data->head->value;
				data->head->value = val;
				// inject tmp after head
				tmp->prev = data->head;
				tmp->next = data->head->next;
				data->head->next = tmp;
			}

			// at start, let current() point to head
			if(!curr)
				curr = data->head;

			sz++;
		}

		Data* begin(void) const { 
			if(data)
				return data->head; 
			return NULL;
		}

		T* first(void) {
			if(data) {
				curr = data->head;
				return curr->value;
			}
			return NULL;
		}

		T* current(void) {
			if(curr)
				return curr->value;
			return NULL;
		}

		T* next(void) {
			if(curr && curr->next) {
				curr = curr->next;
				return curr->value;
			} else {
				// for cases 'for(list.first(); list.current(); list.next())
				curr = NULL;
			}

			return NULL;
		}

		int size(void) { return sz; }
};

#endif
