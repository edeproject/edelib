#include <sys/param.h>
#include <stdio.h>
#include <string.h>
#include <edelib/Run.h>

#include "UnitTest.h"

EDELIB_NS_USE

UT_FUNC(RunSyncTest, "Test run_sync()")
{
	UT_VERIFY( run_sync("/this/file/should/not/exists") == RUN_NOT_FOUND );
	UT_VERIFY( run_sync("/this/file/should/not/exists") == RUN_NOT_FOUND );
	UT_VERIFY( run_sync("this-file-is-local-and-does-not-exists") == RUN_NOT_FOUND );

	UT_VERIFY( run_sync("pwd") == 0 );

	UT_VERIFY( run_sync("./Jamfile") == RUN_NO_ACCESS );
}

UT_FUNC(RunSyncStupidTar, "Test run_sync() + stupid tar")
{
#if defined(BSD)
	/* BSD's get it correctly */
	UT_VERIFY( run_sync("/usr/bin/tar") == 1 );
	UT_VERIFY( run_sync("tar") == 1 );
#elif defined(sun) || defined(__sun)
	/* solaris get it correctly, except 'tar' without path, where GNU version will be preferred if installed */
	UT_VERIFY( run_sync("/usr/bin/tar") == 1 );
#else
	/* tar returns '2' on linux if executed without parameters, which is interpreted as RUN_NOT_FOUND; stupid!!! */
	UT_VERIFY( run_sync("/bin/tar") == RUN_NOT_FOUND );
	UT_VERIFY( run_sync("tar") == RUN_NOT_FOUND );
#endif
}

UT_FUNC(RunAsync, "Test run_async()")
{
	UT_VERIFY( run_async("/this/file/should/not/exists") == RUN_NOT_FOUND );
	UT_VERIFY( run_async("/this/file/should/not/exists") == RUN_NOT_FOUND );
	UT_VERIFY( run_async("this-file-is-local-and-does-not-exists") == RUN_NOT_FOUND );

	/* it always returns 0 when is run in async mode  */
	UT_VERIFY( run_async("pwd") == 0 );

	/* always '0' */
	UT_VERIFY( run_async("tar") == 0 );

	UT_VERIFY( run_async("./Jamfile") == RUN_NO_ACCESS );
}
