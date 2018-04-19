#include <Intriman/Intriman.hpp>

#include <cmath>

#include <pugixml.hpp>

#include <Intriman/Generators/IGenerator.hpp>

namespace Intriman
{

bool ProcessFile(
	std::istream& Stream,
	const std::vector<std::unique_ptr<IGenerator>>& Generators
)
{
	pugi::xml_document Document;
	const pugi::xml_parse_result Parse = Document.load(Stream);

	if( Parse.status != pugi::xml_parse_status::status_ok )
	{
		// Error parsing
		return false;
	}

	const pugi::xml_node IntrinsicsList = Document.first_child();

	for( auto& CurGenerator : Generators )
	{
		CurGenerator->StartList(
			IntrinsicsList.attribute("version").value(),
			IntrinsicsList.attribute("date").value()
		);
	}

	std::size_t CurIndex = 0;
	const std::size_t LastIndex = std::distance(
		Document.first_child().children().begin(),
		Document.first_child().children().end()
	) - 1;

	for( const pugi::xml_node& IntrinsicXML : Document.first_child().children() )
	{
		Intriman::Intrinsic CurIntrinsic;
		CurIntrinsic.Name = IntrinsicXML.attribute("name").value();
		CurIntrinsic.ReturnType = IntrinsicXML.attribute("rettype").value();
		CurIntrinsic.Technology = IntrinsicXML.attribute("tech").value();

		CurIntrinsic.Type = IntrinsicXML.child("type").child_value();
		CurIntrinsic.Category = IntrinsicXML.child("category").child_value();
		// CPUID
		for( pugi::xml_node CurCPUID = IntrinsicXML.child("CPUID");
			CurCPUID;
			CurCPUID = CurCPUID.next_sibling("CPUID")
		)
		{
			CurIntrinsic.CPUID.push_back(CurCPUID.text().get());
		}

		CurIntrinsic.Description = IntrinsicXML.child("description").child_value();
		CurIntrinsic.Operation = IntrinsicXML.child("operation").child_value();
		CurIntrinsic.Header = IntrinsicXML.child("header").child_value();

		// Parameters
		for( pugi::xml_node CurParam = IntrinsicXML.child("parameter");
			CurParam;
			CurParam = CurParam.next_sibling("parameter")
		)
		{
			Intriman::Intrinsic::Parameter NewParam;
			NewParam.Name = CurParam.attribute("varname").value();
			NewParam.Type = CurParam.attribute("type").value();
			CurIntrinsic.Parameters.push_back(NewParam);
		}

		// Instruction
		for( pugi::xml_node CurInstru = IntrinsicXML.child("instruction");
			CurInstru;
			CurInstru = CurInstru.next_sibling("instruction") )
		{
			Intriman::Intrinsic::Instruction NewInstruc;
			NewInstruc.Name = CurInstru.attribute("name").value();
			NewInstruc.Form = CurInstru.attribute("form").value();
			CurIntrinsic.Instructions.push_back(NewInstruc);
		}
		for( auto& CurGenerator : Generators )
		{
			CurGenerator->VisitIntrinsic(CurIntrinsic);
		}

		CurIndex++;
		std::printf(
			"Processing instruction: %8zu/%8zu %%%5.2f%c",
			CurIndex,
			LastIndex,
			(CurIndex / static_cast<std::float_t>(LastIndex)) * 100.0f,
			CurIndex == LastIndex ? '\n':'\r'
		);
		std::fflush(stdout);
	}

	for( auto& CurGenerator : Generators )
	{
		CurGenerator->EndList();
	}

	return true;
}

}
