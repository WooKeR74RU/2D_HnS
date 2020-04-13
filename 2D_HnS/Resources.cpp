#include "Resources.h"

Resources::Resources() : input("input.txt"), output("output.txt")
{ }

Resources& RS()
{
	static Resources resources;
	return resources;
}
