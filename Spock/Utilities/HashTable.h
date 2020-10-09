#pragma once
#include <string>

template <class V>
class HashTable
{
	struct HashChain {
		std::string Key;
		V* Container;
		HashChain* Next;
	};

	private:
		HashTable::HashChain** _internal_list = nullptr;	//the internal list used to hold elements
		unsigned long long int _internal_list_capacity;	//the maximum size that the internal list can be
		unsigned long long int _internal_list_count;	//the current number of elements in the internal list
		
	public:
		
		//Constructors
		inline HashTable() {
			this->_internal_list_capacity = 20;	//the default constructor starts the list at 20
			this->_internal_list_count = 0;

			this->_internal_list = new HashTable::HashChain*[this->_internal_list_capacity];	//create the internal list and clear it
			for (unsigned long long int i = 0; i < this->_internal_list_capacity; ++i) {
				this->_internal_list[i] = nullptr;
			}
		}

		inline HashTable(unsigned long long int capacity) {
			this->_internal_list_capacity = capacity;	//use the given capacity instead
			this->_internal_list_count = 0;

			this->_internal_list = new HashTable::HashChain[this->_internal_list_capacity];	//create the internal list and clear it
			for (unsigned long long int i = 0; i < this->_internal_list_capacity; ++i) {
				this->_internal_list[i] = nullptr;
			}
		}

		//Destructor
		inline ~HashTable() {
			delete[] _internal_list;
		}

		
		unsigned inline long long int Map(std::string key) {
			//found this example here: https://stackoverflow.com/questions/8317508/hash-function-for-a-string
			//pretty basic but credit where credit is due
			//ended up using different prime numbers but the concept is
			//still the same so credit where credit is due I guess
			unsigned long long int hash = 37;	//prime number
			for (int i = 0; i < key.size(); ++i) {
				hash = (hash * 53) ^ ((unsigned long long int)(key[i]) * 73);
			}
			return hash % this->_internal_list_capacity;
		}

		
		
		inline void Insert(std::string key, V* value) {
			unsigned long long int hashKey = this->Map(key);

			//Create the new element to put in the chain
			HashTable::HashChain* c = new HashTable::HashChain;
			c->Container = (V*)value;
			c->Key = key;
			c->Next = nullptr;

			if (this->_internal_list[hashKey] == nullptr) {	//no collision
				this->_internal_list[hashKey] = c;
			}
			else {	//collision, so add to the chain
				HashChain* current = this->_internal_list[hashKey];
				HashChain* prev = nullptr;

				//try to find the end of the chain
				while (current != nullptr) {
					prev = current;
					current = current->Next;
				}
				prev->Next = c;
			}
		}

		inline bool HasKey(std::string key) {
			unsigned long long int hashKey = this->Map(key);

			if (this->_internal_list[hashKey] == nullptr) {
				return false;
			}
			else {
				HashTable::HashChain* c = this->_internal_list[hashKey];
				while (c != nullptr) {
					if (c->Key == key) {
						return true;
					}
					c = c->Next;
				}
				return false;
			}
		}


		inline V* Get(std::string key) {
			unsigned long long int hashKey = this->Map(key);
			if (this->_internal_list[hashKey] == nullptr) {
				return nullptr;
			}
			else {
				HashTable::HashChain* c = this->_internal_list[hashKey];
				while (c != nullptr) {
					if (c->Key == key) {
						return c->Container;
					}
					c = c->Next;
				}
				return nullptr;
			}
		}

		inline void Set(std::string key, V* newValue) {
			unsigned long long int hashKey = this->Map(key);
			if (this->_internal_list[hashKey] == nullptr) {
				return;
			}
			else {
				HashTable::HashChain* c = this->_internal_list[hashKey];
				while (c != nullptr) {
					if (c->Key == key) {
						c->Container = newValue;
					}
					c = c->Next;
				}
				return;
			}
		}

		inline V* Remove(std::string key) {
			unsigned long long int hashKey = this->Map(key);
			if (this->_internal_list[hashKey] == nullptr) {
				return nullptr;
			}
			else {
				HashTable::HashChain* cprev = this->_internal_list[hashKey];
				HashTable::HashChain* c = cprev->Next;

				V* val = nullptr;

				//If the first item in the list is the key then do a swap
				if (cprev->Key == key)
				{
					//first get the value so we can return it later
					val = cprev->Container;
					//then swap the list array
					this->_internal_list[hashKey] = c;

					//then delete the first element
					delete cprev;
					return val;

				}
				else {//otherwise the first element is not the right one so we need to move down the chain
					while (c != nullptr) {
						if (c->Key == key) {//found the key
							//c->Container = newValue;
							val = c->Container;

							//set the next of the previous to be the next of the current
							cprev->Next = c->Next;

							//then delete the current chain link
							delete c;

							return val;

						}
						//make sure the previous one gets set first!
						cprev = c;
						c = c->Next;

					}

					return nullptr;	//if we get here then we didn't find the key in the table
				}
			}
		}


		inline void DebugPrint() {
			for (unsigned long long int i = 0; i < this->_internal_list_capacity; ++i)
			{
				if (this->_internal_list[i] == nullptr) {
					std::cout << "List[" << i << "] is empy\n";
				}
				else {
					std::cout << "List[" << i << "]: \n";
					HashTable::HashChain* c = this->_internal_list[i];
					while (c != nullptr) {
						std::cout << "    " << c->Key << "\n";
						c = c->Next;
					}
				}
			}
		}
};