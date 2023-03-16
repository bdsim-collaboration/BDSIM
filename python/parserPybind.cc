#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;

#include "parser.h"

#include "atom.h"
#include "aperture.h"
#include "beam.h"
#include "element.h"
#include "options.h"
#include "region.h"
#include "fastlist.h"

PYBIND11_MODULE(parser, m) {
    py::class_<GMAD::Parser>(m,"Parser")
      .def_static("Instance",[](std::string fileName) {return GMAD::Parser::Instance(fileName);})
      .def_static("Instance",[](){return GMAD::Parser::Instance();})

       /// Exit method
      .def("quit",&GMAD::Parser::quit)
      /// Method that transfers parameters to element properties
      .def("write_table",&GMAD::Parser::write_table)

       /// Expand a sequence by name from start to end into the target list. This
       /// removes sublines from the beamline into one LINE.
       ///@{ Add value to front of temporary list
       //
       .def("expand_line",[](GMAD::Parser &parser,
                             GMAD::FastList<GMAD::Element>& target,
                             const std::string& name,
                             std::string        start = "",
                             std::string        end   = "") {parser.expand_line(target,name,start,end);})
       /// Expand the main beamline as defined by the use command.
       .def("expand_line",[](GMAD::Parser &parser,
                             const std::string name,
                             std::string start,
                             std::string end) {parser.expand_line(name,start,end);})
       .def("get_sequence",&GMAD::Parser::get_sequence)

       ///@{ Add value to front of temporary list
       .def("Store",[](GMAD::Parser &parser, double value) {parser.Store(value);})
       .def("Store",[](GMAD::Parser &parser, const std::string& name) {parser.Store(name);})

       //
       .def("ClearParams",&GMAD::Parser::ClearParams)

       .def("Add_Atom",[](GMAD::Parser *parser) {parser->Add<GMAD::Atom, GMAD::FastList<GMAD::Atom>>();})
       .def("Add_Aperture",[](GMAD::Parser *parser) {parser->Add<GMAD::Aperture, GMAD::FastList<GMAD::Aperture>>();})
       .def("Add_BLMPlacement",[](GMAD::Parser *parser) {parser->Add<GMAD::BLMPlacement, GMAD::FastList<GMAD::BLMPlacement>>();})
       .def("Add_CavityModel",[](GMAD::Parser *parser) {parser->Add<GMAD::CavityModel, GMAD::FastList<GMAD::CavityModel>>();})
       .def("Add_Crystal",[](GMAD::Parser *parser) {parser->Add<GMAD::Crystal, GMAD::FastList<GMAD::Crystal>>();})
       .def("Add_Field",[](GMAD::Parser *parser) {parser->Add<GMAD::Field, GMAD::FastList<GMAD::Field>>();})

       .def("Add_Atom",[](GMAD::Parser *parser, bool unique, std::string className) {parser->Add<GMAD::Atom, GMAD::FastList<GMAD::Atom>>(unique, className);})
       .def("Add_Aperture",[](GMAD::Parser *parser, bool unique, std::string className) {parser->Add<GMAD::Aperture, GMAD::FastList<GMAD::Aperture>>(unique, className);})
       .def("Add_BLMPlacement",[](GMAD::Parser *parser, bool unique, std::string className) {parser->Add<GMAD::BLMPlacement, GMAD::FastList<GMAD::BLMPlacement>>(unique, className);})
       .def("Add_CavityModel",[](GMAD::Parser *parser, bool unique, std::string className) {parser->Add<GMAD::CavityModel, GMAD::FastList<GMAD::CavityModel>>(unique, className);})
       .def("Add_Crystal",[](GMAD::Parser *parser, bool unique, std::string className) {parser->Add<GMAD::Crystal, GMAD::FastList<GMAD::Crystal>>(unique, className);})
       .def("Add_Field",[](GMAD::Parser *parser, bool unique, std::string className) {parser->Add<GMAD::Field, GMAD::FastList<GMAD::Field>>(unique, className);})

       .def("GetGlobal_Atom",[](GMAD::Parser *parser) {return parser->GetGlobalPtr<GMAD::Atom>();}, py::return_value_policy::automatic_reference)
       .def("GetGlobal_Aperture",[](GMAD::Parser *parser) {return parser->GetGlobalPtr<GMAD::Aperture>();})
       .def("GetGlobal_Beam",[](GMAD::Parser *parser) {return parser->GetGlobalPtr<GMAD::Beam>();})
       .def("GetGlobal_BLMPlacement",[](GMAD::Parser *parser) {return parser->GetGlobalPtr<GMAD::BLMPlacement>();})
       .def("GetGlobal_CavityModel",[](GMAD::Parser *parser) {return parser->GetGlobalPtr<GMAD::CavityModel>();})
       .def("GetGlobal_Crystal",[](GMAD::Parser parser) {return parser.GetGlobal<GMAD::Crystal>();})
       .def("GetGlobal_Field",[](GMAD::Parser *parser) {return parser->GetGlobalPtr<GMAD::Field>();})

       .def("GetGlobal_Material",[](GMAD::Parser parser) {return parser.GetGlobal<GMAD::Material>();})
       .def("GetGlobal_NewColour",[](GMAD::Parser parser) {return parser.GetGlobal<GMAD::NewColour>();})
       .def("GetGlobal_Options",[](GMAD::Parser parser) {return parser.GetGlobal<GMAD::Options>();})
       .def("GetGlobal_Parameters",[](GMAD::Parser parser) {return parser.GetGlobal<GMAD::Parameters>();})
       .def("GetGlobal_Placement",[](GMAD::Parser parser) {return parser.GetGlobal<GMAD::Placement>();})
       .def("GetGlobal_Query",[](GMAD::Parser parser) {return parser.GetGlobal<GMAD::Query>();})
       .def("GetGlobal_Region",[](GMAD::Parser parser) {return parser.GetGlobal<GMAD::Region>();})
       .def("GetGlobal_SpamplerPlacement",[](GMAD::Parser parser) {return parser.GetGlobal<GMAD::SamplerPlacement>();})
       .def("GetGlobal_Scorer",[](GMAD::Parser parser) {return parser.GetGlobal<GMAD::Scorer>();})
       .def("GetGlobal_ScorerMesh",[](GMAD::Parser parser) {return parser.GetGlobal<GMAD::ScorerMesh>();})
       .def("GetGlobal_Tunnel",[](GMAD::Parser parser) {return parser.GetGlobal<GMAD::Tunnel>();})

       .def("GetList_Atom",[](GMAD::Parser *parser) {return parser->GetList<GMAD::Atom, GMAD::FastList<GMAD::Atom>>();})
       .def("GetList_Aperture",[](GMAD::Parser *parser) {return parser->GetList<GMAD::Aperture, GMAD::FastList<GMAD::Aperture>>();})
       .def("GetList_BLMPlacement",[](GMAD::Parser *parser) {return parser->GetList<GMAD::BLMPlacement, GMAD::FastList<GMAD::BLMPlacement>>();})
       .def("GetList_CavityModel",[](GMAD::Parser *parser) {return parser->GetList<GMAD::CavityModel, GMAD::FastList<GMAD::CavityModel>>();})
       .def("GetList_Crystal",[](GMAD::Parser *parser) {return parser->GetList<GMAD::Crystal, GMAD::FastList<GMAD::Crystal>>();})
       .def("GetList_Field",[](GMAD::Parser *parser) {return parser->GetList<GMAD::Field, GMAD::FastList<GMAD::Field>>();})

       .def("SetValue_Atom",[](GMAD::Parser &parser, std::string property, std::string value ) {parser.SetValue<GMAD::Atom,std::string>(property, value);})
       .def("SetValue_Atom",[](GMAD::Parser &parser, std::string property, double value ) {parser.SetValue<GMAD::Atom,double>(property, value);})

       .def("PrintBeamline", &GMAD::Parser::PrintBeamline)
       .def("PrintElements", &GMAD::Parser::PrintElements)
       .def("PrintOptions", &GMAD::Parser::PrintOptions)
       //
       .def("TryPrintingObject", &GMAD::Parser::TryPrintingObject)
       //
       .def("GetBeamline",&GMAD::Parser::GetBeamline);
}