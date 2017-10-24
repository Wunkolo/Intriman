#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstdlib>

#include <iostream>

#include <pugixml.hpp>

int main( int argc, const char* argv[] )
{
	pugi::xml_document Document;
	pugi::xml_parse_result Parse = Document.load_file(argv[1]);

	for( pugi::xml_node Intrinsic : Document.first_child().children() )
	{
		std::cout << Intrinsic.name() << std::endl;
		std::cout << '\t' << Intrinsic.attribute("name").value() << std::endl;
		for( pugi::xml_node Child : Intrinsic.children() )
		{
			std::cout << '\t' << Child.name() << "\t|\t" << Child.child_value() << std::endl;
		}
	}

	return EXIT_SUCCESS;
}
