#ifndef __UNITTEST_H__
#define __UNITTEST_H__

struct UTMsgList {
	char* msg;
	char* file;
	unsigned long line;
	UTMsgList* first;
	UTMsgList* next;
};

class UnitTest {
	private:
		char* tname;
		char* tdescr;
		int   tfails;
		int   tpasses;
		int   mnumber;
		UTMsgList* mlist;

	protected:
		virtual void run(void) = 0;

	public:
		UnitTest(const char* name, const char* descr);
		virtual ~UnitTest();
		int failed(void) { return tfails; }
		int passed(void) { return tpasses; }
		const char* name(void)          { return tname; }
		const char* description(void)   { return tdescr; }
		const UTMsgList*  msglist(void);
		int msglist_size(void) { return mnumber; }

		void execute(void);
		void add_message(const char* msg);
		void add_message(const char* msg, const char* file, unsigned long line);

		void verify(bool condition, const char* condstr, const char* fname, unsigned long line);
};

#define UT_VERIFY(cond)                      verify(cond,  #cond, __FILE__, __LINE__)
#define UT_VERIFY_EQUAL(p1, p2)              verify(((p1) == (p2)),  #p1" == "#p2, __FILE__, __LINE__)
#define UT_VERIFY_NOT_EQUAL(p1, p2)          verify(((p1) != (p2)),  #p1" != "#p2, __FILE__, __LINE__)
#define UT_FAIL(reason)                      verify(false, reason, __FILE__, __LINE__)

struct UTList {
	bool alloc;
	UnitTest* test;
	UTList* first;
	UTList* next;
};

class UnitTestSuite {
	private:
		int ntests;
		UTList* tlist;

	public:
		UnitTestSuite();
		~UnitTestSuite();
		static UnitTestSuite& global(void);
		void add(UnitTest* t, bool alloc = true);
		int run(bool verbose, bool cdash_output);
		int tests(void) { return ntests; }
};

#define UT_FUNC(Name, description)                         \
class UT##Name : public UnitTest                           \
{                                                          \
	public:                                                \
		static UT##Name instance;                          \
		UT##Name() : UnitTest(#Name, description)          \
		{                                                  \
			UnitTestSuite::global().add(this, false);      \
		}                                                  \
		~UT##Name() {}                                     \
		void run(void);                                    \
};                                                         \
UT##Name UT##Name::instance;                               \
                                                           \
void UT##Name::run(void)                                   

#endif
