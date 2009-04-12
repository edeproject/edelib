#include <stdio.h>
#include <string.h>
#include <edelib/TiXml.h>
#include <edelib/String.h>

#include "UnitTest.h"

EDELIB_NS_USE

#define CCHARP(str)           ((const char*)str)
#define STR_EQUAL(str1, str2) (strcmp(CCHARP(str1), CCHARP(str2)) == 0)

UT_FUNC(XmlTest, "Test XML")
{
	TiXmlDocument doc("test.xml");
	if(!doc.LoadFile()) {
		UT_FAIL("No test.xml, but expected to be");
		return;
	}

	TiXmlNode *elem = doc.FirstChild("start");
	for(elem = elem->FirstChildElement(); elem; elem = elem->NextSibling()) {
		if(STR_EQUAL(elem->Value(), "node1")) {
			const char* vv = elem->ToElement()->Attribute("param");
			UT_VERIFY( STR_EQUAL(vv, "value1") );
		}

		if(STR_EQUAL(elem->Value(), "node2")) {
			const char* vv = elem->ToElement()->Attribute("param");
			UT_VERIFY( STR_EQUAL(vv, "value2") );
		}
	}
}
