#include "Resources.h"

Resources::Resources() : input("Resources/input.txt"), output("Resources/output.txt")
{ }

Resources& RS()
{
	static Resources resources;
	return resources;
}
