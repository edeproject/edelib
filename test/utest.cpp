#include "UnitTest.h"

class MyUnitTest : public UnitTest
{
	public:
		MyUnitTest() : UnitTest("MyUnitTest", "MyUnitTest") { }
		void run()
		{
			UT_VERIFY(2 == 2);
			UT_VERIFY(2 == 2);
			UT_VERIFY(2 == 2);
			UT_VERIFY(2 == 2);
			UT_VERIFY(2 == 2);
			UT_VERIFY(5 == 2);
		}
};

class MyUnitTest2 : public UnitTest
{
	public:
		MyUnitTest2() : UnitTest("MyUnitTest2", "MyUnitTest2") { }
		void test_verify_pass()
		{
			UT_VERIFY(2 == 2);
			UT_VERIFY(3 == 3);
			UT_VERIFY('a' == 'a');
			UT_VERIFY(true);
			UT_VERIFY(!false);

			int a = 2;
			UT_VERIFY_EQUAL(++a, 3);
		}

		void test_verify_fail()
		{
			UT_VERIFY(3-1 != 2);
			UT_VERIFY(4+2 == 2);
			UT_VERIFY(1-0 == 3);
			int a = 2;
			UT_VERIFY_NOT_EQUAL(a++, 2);
		}

		void run()
		{
			test_verify_pass();
			test_verify_fail();
		}
};

UT_FUNC(UnitTestTest, "Test UnitTest")
{
	MyUnitTest m;
	m.run();

	UT_VERIFY( m.failed() == 1 );
	UT_VERIFY( m.passed() == 5 );
}

UT_FUNC(UnitTestTest2, "Test UnitTest")
{
	MyUnitTest2 m;
	m.run();
	UT_VERIFY( m.failed() == 4 );
	UT_VERIFY( m.passed() == 6 );
}

UT_FUNC(UnitTestSuite, "Test UnitTestSuite")
{
	UnitTestSuite s;
	s.add(new MyUnitTest);
	s.add(new MyUnitTest2);
	MyUnitTest2 m;
	s.add(&m, false);
	UT_VERIFY(s.tests() == 3);
}
