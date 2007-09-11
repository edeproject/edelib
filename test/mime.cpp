#include <edelib/MimeType.h>
#include "UnitTest.h"
#include <stdio.h>

using namespace edelib;

UT_FUNC(MimeTypeTest, "Test MimeType")
{
	MimeType mt;
	UT_VERIFY( mt.set("mime.cpp") == true );
	UT_VERIFY( mt.type() == "text/x-c++src" );
	UT_VERIFY( mt.comment() == "C++ source code" );
	UT_VERIFY( mt.icon_name() == "text-x-c++src" );

	// unrecognized
	mt.set("Jamfile");
	UT_VERIFY( mt.type() == "application/octet-stream" );
	UT_VERIFY( mt.comment() == "unknown" );
	UT_VERIFY( mt.icon_name() == "application-octet-stream" );

	mt.set("mime.o");
	UT_VERIFY( mt.type() == "application/x-object" );
	UT_VERIFY( mt.comment() == "object code" );
	UT_VERIFY( mt.icon_name() == "application-x-object" );

	mt.set("../doc/footer.html");
	UT_VERIFY( mt.type() == "text/html" );
	UT_VERIFY( mt.icon_name() == "text-html" );

	mt.set("../doc/genreadme");
	UT_VERIFY( mt.type() == "application/x-awk" );
	UT_VERIFY( mt.comment() == "AWK script" );
	UT_VERIFY( mt.icon_name() == "application-x-awk" );
	UT_VERIFY( mt.subclass_of("application/x-executable") );

	// duplicate, should not run tokenizer
	UT_VERIFY( mt.icon_name() == "application-x-awk" );
	UT_VERIFY( mt.icon_name() == "application-x-awk" );
	UT_VERIFY( mt.icon_name() == "application-x-awk" );
	
	// duplicate, should not reload comment
	UT_VERIFY( mt.comment() == "AWK script" );
	UT_VERIFY( mt.comment() == "AWK script" );
	UT_VERIFY( mt.comment() == "AWK script" );
	UT_VERIFY( mt.comment() == "AWK script" );

	// empty mime, do nothing
	MimeType mt2;
	UT_VERIFY( mt2.type() == "" );
	UT_VERIFY( mt2.comment() == "" );
	UT_VERIFY( mt2.icon_name() == "" );

	// folder;
	mt.set("/");
	UT_VERIFY( mt.type() == "inode/directory" );
	UT_VERIFY( mt.icon_name() == "folder" );


	/* TODO
	UT_VERIFY( mt.set("this-does-not-exists") == false );
	UT_VERIFY( mt.type().empty() == true );
	UT_VERIFY( mt.comment().empty() == true );
	UT_VERIFY( mt.icon_name().empty() == true );
	*/
}
