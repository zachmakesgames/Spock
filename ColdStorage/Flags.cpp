#include "Flags.h"


//Initializer and destructor
//Just some basic initialization stuff
Flags::Flags() {
	this->flagList = new Flag[this->maxFlags];
}
Flags::~Flags() {
	if (this->flagList != nullptr) {
		delete[] this->flagList;
	}
}

//Member function to add a flag to the list of flags
//Checks if a flag of that type already exists to avoid
//duplicate flags. If multiple flags of the same type are
//added only the first one gets added.
bool Flags::AddFlag(Flag f) {

	//check that the flag is not of type EMPTY, we dont want
	//any empty flags in the list
	if (f.t == FlagType::EMPTY) {
		return false;
	}

	//check for existing and return false if it already exists.
	if (this->HasFlag(f.t)) {
		return false;
	}

	//Check for space in the flags list and resize if necessary
	if (this->flagCount >= maxFlags) {
		this->Resize();
	}

	//Then add the flag to the list and increment the counter
	this->flagList[this->flagCount++] = f;

	//Return true since the operation succeeded
	return true;
}

//Member function to check if a flag of type t already
//exists in the list of flags.
bool Flags::HasFlag(FlagType t) {

	//Shortcircuit if the list is emtpy
	if (this->flagCount == 0) {
		return false;
	}

	//Loop through the flags and look for one with the specified type
	//Return true when we find it.
	for (int i = 0; i < this->flagCount; ++i) {
		if (this->flagList[i].t == t) {
			return true;
		}
	}

	//If we break out of the for loop then we didnt find one,
	//so return false.
	return false;
}

//Member function to retreive a flag of type t from
//the list if it exists. Returns a flag of type EMPTY
//if the specified flag type does not exist in the list
Flag Flags::GetFlag(FlagType t) {
	//Shortcircuit if the list is emtpy
	if (this->flagCount == 0) {
		return { FlagType::EMPTY, "" };
	}

	//Loop through the flags and look for one with the specified type
	//Return true when we find it.
	for (int i = 0; i < this->flagCount; ++i) {
		if (this->flagList[i].t == t) {
			return this->flagList[i];
		}
	}

	//If we break out of the for loop then we didnt find one,
	//so return false.
	return { FlagType::EMPTY, "" };
}

//Private member function to increase the size of the list
//for more space. Currently 
void Flags::Resize() {

	//Create the new list twice as big
	uint32_t newMaxSize = this->maxFlags * 2;
	Flag* newList = new Flag[newMaxSize];

	//Populate the new list with existing flags
	for (int i = 0; i < this->flagCount; ++i) {
		newList[i] = this->flagList[i];
	}

	//delete the old list
	delete[] this->flagList;

	//Swap in the new list
	this->flagList = newList;

	//Set the new max size
	this->maxFlags = newMaxSize;

}

//A member function to get the number of flags defined in the list
uint32_t Flags::Count() {
	return this->flagCount;
}