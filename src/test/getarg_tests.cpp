#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-hyi");
    BOOST_CHECK(GetBoolArg("-hyi"));
    BOOST_CHECK(GetBoolArg("-hyi", false));
    BOOST_CHECK(GetBoolArg("-hyi", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-hyio"));
    BOOST_CHECK(!GetBoolArg("-hyio", false));
    BOOST_CHECK(GetBoolArg("-hyio", true));

    ResetArgs("-hyi=0");
    BOOST_CHECK(!GetBoolArg("-hyi"));
    BOOST_CHECK(!GetBoolArg("-hyi", false));
    BOOST_CHECK(!GetBoolArg("-hyi", true));

    ResetArgs("-hyi=1");
    BOOST_CHECK(GetBoolArg("-hyi"));
    BOOST_CHECK(GetBoolArg("-hyi", false));
    BOOST_CHECK(GetBoolArg("-hyi", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-nohyi");
    BOOST_CHECK(!GetBoolArg("-hyi"));
    BOOST_CHECK(!GetBoolArg("-hyi", false));
    BOOST_CHECK(!GetBoolArg("-hyi", true));

    ResetArgs("-nohyi=1");
    BOOST_CHECK(!GetBoolArg("-hyi"));
    BOOST_CHECK(!GetBoolArg("-hyi", false));
    BOOST_CHECK(!GetBoolArg("-hyi", true));

    ResetArgs("-hyi -nohyi");  // -hyi should win
    BOOST_CHECK(GetBoolArg("-hyi"));
    BOOST_CHECK(GetBoolArg("-hyi", false));
    BOOST_CHECK(GetBoolArg("-hyi", true));

    ResetArgs("-hyi=1 -nohyi=1");  // -hyi should win
    BOOST_CHECK(GetBoolArg("-hyi"));
    BOOST_CHECK(GetBoolArg("-hyi", false));
    BOOST_CHECK(GetBoolArg("-hyi", true));

    ResetArgs("-hyi=0 -nohyi=0");  // -hyi should win
    BOOST_CHECK(!GetBoolArg("-hyi"));
    BOOST_CHECK(!GetBoolArg("-hyi", false));
    BOOST_CHECK(!GetBoolArg("-hyi", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--hyi=1");
    BOOST_CHECK(GetBoolArg("-hyi"));
    BOOST_CHECK(GetBoolArg("-hyi", false));
    BOOST_CHECK(GetBoolArg("-hyi", true));

    ResetArgs("--nohyi=1");
    BOOST_CHECK(!GetBoolArg("-hyi"));
    BOOST_CHECK(!GetBoolArg("-hyi", false));
    BOOST_CHECK(!GetBoolArg("-hyi", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-hyi", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-hyi", "eleven"), "eleven");

    ResetArgs("-hyi -bar");
    BOOST_CHECK_EQUAL(GetArg("-hyi", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-hyi", "eleven"), "");

    ResetArgs("-hyi=");
    BOOST_CHECK_EQUAL(GetArg("-hyi", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-hyi", "eleven"), "");

    ResetArgs("-hyi=11");
    BOOST_CHECK_EQUAL(GetArg("-hyi", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-hyi", "eleven"), "11");

    ResetArgs("-hyi=eleven");
    BOOST_CHECK_EQUAL(GetArg("-hyi", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-hyi", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-hyi", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-hyi", 0), 0);

    ResetArgs("-hyi -bar");
    BOOST_CHECK_EQUAL(GetArg("-hyi", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-hyi=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-hyi", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-hyi=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-hyi", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--hyi");
    BOOST_CHECK_EQUAL(GetBoolArg("-hyi"), true);

    ResetArgs("--hyi=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-hyi", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-nohyi");
    BOOST_CHECK(!GetBoolArg("-hyi"));
    BOOST_CHECK(!GetBoolArg("-hyi", true));
    BOOST_CHECK(!GetBoolArg("-hyi", false));

    ResetArgs("-nohyi=1");
    BOOST_CHECK(!GetBoolArg("-hyi"));
    BOOST_CHECK(!GetBoolArg("-hyi", true));
    BOOST_CHECK(!GetBoolArg("-hyi", false));

    ResetArgs("-nohyi=0");
    BOOST_CHECK(GetBoolArg("-hyi"));
    BOOST_CHECK(GetBoolArg("-hyi", true));
    BOOST_CHECK(GetBoolArg("-hyi", false));

    ResetArgs("-hyi --nohyi");
    BOOST_CHECK(GetBoolArg("-hyi"));

    ResetArgs("-nohyi -hyi"); // hyi always wins:
    BOOST_CHECK(GetBoolArg("-hyi"));
}

BOOST_AUTO_TEST_SUITE_END()
