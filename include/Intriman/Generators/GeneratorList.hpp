/*
* Usage:
*		#undef GEN_ENTRY
*		#define GEN_ENTRY(GEN_NAME) \
*			// stuff you want to do per-GEN-entry
*			// use "##GEN_NAME##"
*			// ex: SomeVectorOfGenerators.push_back("##GEN_NAME##");
*
*		#include "Generators.hpp"
*		#undef GEN_ENTRY
*/

GEN_ENTRY(Text)
GEN_ENTRY(Roff)
GEN_ENTRY(Markdown)