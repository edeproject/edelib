#include <edelib/TiXml.h>
#include <edelib/String.h>
#include "UnitTest.h"
#include <stdio.h>

using namespace edelib;

UT_FUNC(XmlTest, "Test XML")
{
	TiXmlDocument doc("test.xml");
	if(!doc.LoadFile())
	{
		UT_FAIL("No test.xml, but expected to be");
		return;
	}

	TiXmlNode *elem = doc.FirstChild("start");
	for(elem = elem->FirstChildElement(); elem; elem = elem->NextSibling()) 
	{
		if(elem->Value() == "node1") 
		{
			const char* vv = elem->ToElement()->Attribute("param");
			UT_VERIFY( vv == "value1");
		}

		if(elem->Value() == "node2") 
		{
			const char* vv = elem->ToElement()->Attribute("param");
			UT_VERIFY( vv == "value2");
		}

	}
}
