/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2017.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published 
by the Free Software Foundation version 3 of the License.

BDSIM is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BDSIM.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "BDSGDMLPreprocessor.hh"
#include "BDSTemporaryFiles.hh"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>

#include "globals.hh"

#include <algorithm>
#include <map>
#include <string>
#include <vector>


using namespace xercesc;

G4String BDS::PreprocessGDML(const G4String& file,
			     const G4String& prefix)
{
  try
    {XMLPlatformUtils::Initialize();}
  catch (const XMLException& toCatch)
    {
      char* message = XMLString::transcode(toCatch.getMessage());
      G4cout << "Error during initialization! :\n"
	   << message << "\n";
      XMLString::release(&message);
      exit(1);
    }
  
  XercesDOMParser* parser = new XercesDOMParser();
  parser->setValidationScheme(XercesDOMParser::Val_Always);
  parser->setDoNamespaces(true);    // optional

  ErrorHandler* errHandler = (ErrorHandler*) new HandlerBase();
  parser->setErrorHandler(errHandler);

  //  char* xmlFile = "./iba/GDML/B1G_Coils.gdml;
  //char *prefix  = args[2];

  try
    {parser->parse(file.c_str());}
  catch (const XMLException& toCatch)
    {
      char* message = XMLString::transcode(toCatch.getMessage());
      G4cout << "Exception message is: \n"
	   << message << "\n";
      XMLString::release(&message);
      exit(1);
    }
  catch (const DOMException& toCatch)
    {
      char* message = XMLString::transcode(toCatch.msg);
      G4cout << "Exception message is: \n"
	   << message << "\n";
      XMLString::release(&message);
      exit(1);
    }
  catch (...)
    {
    G4cout << "Unexpected Exception \n" ;
    return -1;
    }
  
  // walk through all nodes to extract names and attributes
  DOMDocument* doc           = parser->getDocument();
  DOMElement* docRootNode    = doc->getDocumentElement();
  DOMNodeIterator* docWalker = doc->createNodeIterator(docRootNode, DOMNodeFilter::SHOW_ELEMENT,NULL,true);

  std::vector<std::string> names;
  std::map<std::string,int> count;  

  for (DOMNode* current_node = docWalker->nextNode(); current_node != 0; current_node = docWalker->nextNode())
    {
      std::string thisNodeName = XMLString::transcode(current_node->getNodeName());
      DOMNamedNodeMap* attrMap = current_node->getAttributes();
      
      // loop over attributes
      for(int i = 0; i < static_cast<int>(attrMap->getLength()); i++)
	{
	  DOMNode* attr = attrMap->item(i);
	  // cout << thisNodeName << " " << XMLString::transcode(attr->getNodeName()) << " " <<XMLString::transcode(attr->getNodeValue()) << std::endl;
	  std::string name = XMLString::transcode(attr->getNodeValue());
	  
	  if(XMLString::compareIString(attr->getNodeName(), XMLString::transcode("name")) == 0)
	    {
	      names.push_back(name);
	      count[name] = 0;
	    }
	  else
	    {
	      count[name]++;
	    }
	}
    } 
  
  // print label and reference usage
  /*for(const auto& i : names)
    {
      G4cout << i << " " << G4endl;
    }*/

  // walk through nodes again to replace names and references
  docWalker->detach();
  docWalker = doc->createNodeIterator(docRootNode, DOMNodeFilter::SHOW_ELEMENT,nullptr,true);
  
  for (DOMNode* current_node = docWalker->nextNode(); current_node != 0; current_node = docWalker->nextNode())
    {
      DOMNamedNodeMap* attrMap = current_node->getAttributes();
      
      // loop over attributes
      for(int i = 0; i < static_cast<int>(attrMap->getLength()); i++)
	{
	  DOMNode* attr = attrMap->item(i);
	  std::string name = XMLString::transcode(attr->getNodeValue());
	  
	  if(XMLString::compareIString(attr->getNodeName(),XMLString::transcode("name")) == 0)
	    {
	      std::string newName = prefix + "_" + name;
	      attr->setNodeValue(XMLString::transcode(name.c_str()));
	    }
	  else
	    {
	      std::string value = XMLString::transcode(attr->getNodeValue());
	      if(find(names.begin(), names.end(), value) != names.end()) {
		attr->setNodeValue(XMLString::transcode((prefix + "_" + value).c_str()));
	      }
	    }
	}
    }
  
  docWalker->detach();

  // write file from DOM
  DOMImplementation* pImplement        = DOMImplementationRegistry::getDOMImplementation(XMLString::transcode("LS"));
  DOMLSSerializer*   pSerializer       = ((DOMImplementationLS*)pImplement)->createLSSerializer();
  DOMConfiguration*  pDomConfiguration = pSerializer->getDomConfig();
  pDomConfiguration->setParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true);
  XMLFormatTarget*   pTarget           = new LocalFileFormatTarget("./test.xml");
  DOMLSOutput*       pDomLsOutput      = ((DOMImplementationLS*)pImplement)->createLSOutput();
  pDomLsOutput->setByteStream(pTarget);  
  pSerializer->write(doc, pDomLsOutput);

  delete parser;
  delete errHandler;
  return G4String("temp");
}


