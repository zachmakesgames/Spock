#pragma once
#include <stdint.h>
#include <string.h>

enum class FlagType {
	FT_TYPE,
	FT_OUTPUT,
	FT_NOPACK,
	FT_VERIFY,
	FT_THREAD,
	FT_INCLUDE,
	FT_RECURSE,
	EMPTY
};


//Design note: Using a fixed character array here to make
//dealing with the dynamic array in the Flags class much easier.
//No flag should be longer than 256 characters anyways, that would
//just be stupid.
/*typedef struct Flag {
	FlagType t;			//The flag type
	char text[256];		//The text value of the flag

}Flag;
*/

class Flag {
	public:
		FlagType t;
		char text[256];

		inline friend bool operator==(const Flag& lflag, const Flag& rflag) {
			return (lflag.t == rflag.t && 
				strcmp(lflag.text, rflag.text) == 0);
		}

};

class Flags
{
	public:
		Flags();
		~Flags();
		bool AddFlag(Flag f);
		bool HasFlag(FlagType t);
		Flag GetFlag(FlagType t);
		uint32_t Count();

		inline Flags operator=(Flags& newflags) {
			Flags f;
			if (f.flagList != nullptr) {
				delete[] f.flagList;
			}
			
			f.flagList = new Flag[f.maxFlags];
			f.flagCount = newflags.Count();
			for (int i = 0; i < newflags.Count(); ++i) {
				f.flagList[i] = newflags.flagList[i];
			}
			return f;
		}
		

	protected:
		void Resize();

		Flag* flagList = nullptr;
		uint32_t flagCount = 0;
		uint32_t maxFlags = 10;
};

