#include <Intriman/Intriman.hpp>

#include <cmath>

#include <pugixml.hpp>

#include <Intriman/Generators/IGenerator.hpp>

namespace Intriman
{

bool ProcessFile(
	std::istream& IntrinsicsFile,
	std::istream& LatencyFile,
	const std::vector<std::unique_ptr<IGenerator>>& Generators
)
{
	// Parse Intrinsics
	pugi::xml_document IntrinsicDocument;
	const pugi::xml_parse_result IntrinsicXML = IntrinsicDocument.load(IntrinsicsFile);

	if( IntrinsicXML.status != pugi::xml_parse_status::status_ok )
	{
		// Error parsing
		return false;
	}

	const pugi::xml_node IntrinsicsList = IntrinsicDocument.first_child();

	for( auto& CurGenerator : Generators )
	{
		CurGenerator->StartList(
			IntrinsicsList.attribute("version").value(),
			IntrinsicsList.attribute("date").value()
		);
	}

	std::size_t CurIndex = 0;
	const std::size_t LastIndex = std::distance(
		IntrinsicDocument.first_child().children().begin(),
		IntrinsicDocument.first_child().children().end()
	) - 1;

	for( const pugi::xml_node& IntrinsicNode : IntrinsicDocument.first_child().children() )
	{
		Intriman::Intrinsic CurIntrinsic;
		CurIntrinsic.Name = IntrinsicNode.attribute("name").value();
		CurIntrinsic.ReturnType = IntrinsicNode.attribute("rettype").value();
		CurIntrinsic.Technology = IntrinsicNode.attribute("tech").value();

		CurIntrinsic.Type = IntrinsicNode.child("type").child_value();
		CurIntrinsic.Category = IntrinsicNode.child("category").child_value();
		// CPUID
		for( pugi::xml_node CurCPUID = IntrinsicNode.child("CPUID");
			CurCPUID;
			CurCPUID = CurCPUID.next_sibling("CPUID")
		)
		{
			CurIntrinsic.CPUID.push_back(CurCPUID.text().get());
		}

		CurIntrinsic.Description = IntrinsicNode.child("description").child_value();
		CurIntrinsic.Operation = IntrinsicNode.child("operation").child_value();
		CurIntrinsic.Header = IntrinsicNode.child("header").child_value();

		// Parameters
		for( pugi::xml_node CurParam = IntrinsicNode.child("parameter");
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
		for( pugi::xml_node InstructionNode = IntrinsicNode.child("instruction");
			InstructionNode;
			InstructionNode = InstructionNode.next_sibling("instruction") )
		{
			Intriman::Intrinsic::Instruction NewInstruc;
			NewInstruc.Name = InstructionNode.attribute("name").value();
			NewInstruc.Operands = InstructionNode.attribute("form").value();
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
