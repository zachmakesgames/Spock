#pragma once
#include <stdint.h>

enum class FlagType {
	FT_TYPE,
	FT_OUTPUT,
	FT_NOPACK,
	FT_VERIFY,
	FT_THREAD,
	FT_INCLUDE,
	EMPTY
};


//Design note: Using a fixed character array here to make
//dealing with the dynamic array in the Flags class much easier.
//No flag should be longer than 256 characters anyways, that would
//just be stupid.
typedef struct Flag {
	FlagType t;			//The flag type
	char text[256];		//The text value of the flag

}Flag;

class Flags
{
	public:
		Flags();
		~Flags();
		bool AddFlag(Flag f);
		bool HasFlag(FlagType t);
		Flag GetFlag(FlagType t);
		uint32_t Count();

	private:
		void Resize();

		Flag* flagList = nullptr;
		uint32_t flagCount = 0;
		uint32_t maxFlags = 10;
};

