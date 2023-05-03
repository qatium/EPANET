/*
 ******************************************************************************
 Project:      OWA EPANET
 Version:      2.2
 Module:       test_control.cpp
 Description:  Tests EPANET toolkit api functions
 Authors:      see AUTHORS
 Copyright:    see AUTHORS
 License:      see LICENSE
 Last Updated: 03/21/2019
 ******************************************************************************
*/

/*
This is a test for the API functions that adds rules and deletes
nodes and links from a project. Deletion can be conditional on
node or link appearing in any simple or rule-based controls.
*/

#include <boost/test/unit_test.hpp>

#include "test_toolkit.hpp"


char R1[] = "RULE 1 \n IF NODE 2 LEVEL < 100 \n THEN LINK 9 STATUS = OPEN";
char R2[] = "RULE 2\nIF SYSTEM TIME = 4\nTHEN LINK 9 STATUS = CLOSED\nAND LINK 31 STATUS = CLOSED";
char R3[] = "RULE 3\nIF NODE 23 PRESSURE ABOVE 140\nAND NODE 2 LEVEL > 120\n"
            "THEN LINK 113 STATUS = CLOSED\nELSE LINK 22 STATUS = CLOSED";


BOOST_AUTO_TEST_SUITE (test_controls)

BOOST_FIXTURE_TEST_CASE(test_add_get_rule,  FixtureOpenClose)
{
    int ruleCount, nP, nTA, nEA;
    int link113, node23, link22, pump9_before, pump9_after;
    double priority;


    // Add the 3 rules to the project
    error = EN_addrule(ph, R1);
    BOOST_REQUIRE(error == 0);
    error = EN_addrule(ph, R2);
    BOOST_REQUIRE(error == 0);
    error = EN_addrule(ph, R3);
    BOOST_REQUIRE(error == 0);

    // Check that rules were added
    error = EN_getcount(ph, EN_RULECOUNT, &ruleCount);
    BOOST_CHECK(ruleCount == 3);

    // Check the number of clauses in rule 3
    error = EN_getrule(ph, 3, &nP, &nTA, &nEA, &priority);
    BOOST_CHECK(nP == 2);
    BOOST_CHECK(nTA == 1);
    BOOST_CHECK(nTA == 1);

    // Try to delete link 113 conditionally which will fail
    // because it's in rule 3
    EN_getlinkindex(ph, (char *)"113", &link113);
    error = EN_deletelink(ph, link113, EN_CONDITIONAL);
    BOOST_REQUIRE(error == 261);

    // Delete node 23 unconditionally which will result in the
    // deletion of rule 3 as well as links 22 and 113
    EN_getnodeindex(ph, (char *)"23", &node23);
    EN_getlinkindex(ph, (char *)"22", &link22);
    EN_getlinkindex(ph, (char *)"9", &pump9_before);
    error = EN_deletenode(ph, node23, EN_UNCONDITIONAL);
    BOOST_REQUIRE(error == 0);

    // Check that there are now only 2 rules
    error = EN_getcount(ph, EN_RULECOUNT, &ruleCount);
    BOOST_CHECK(ruleCount == 2);

    // Check that link 22 no longer exists
    error = EN_getlinkindex(ph, (char *)"22", &link22);
    BOOST_CHECK(error > 0);

    // Check that the index of pump9 has been reduced by 2
    error = EN_getlinkindex(ph, (char *)"9", &pump9_after);
    BOOST_CHECK(pump9_before - pump9_after == 2);
}

BOOST_AUTO_TEST_CASE(test_controls_timed_from_until)
{
    int type = 0, linkIndex = 0, nodeIndex = 0, controlCount = 0, error = 0;
    int pumpIndex = 0;
    double setting = 0.0, level = 0.0, fromTime = 0.0, untilTime = 0.0, pumpSetting = 0.0;
    long tstep = 0, t = 0;

    EN_Project ph = NULL;

    EN_createproject(&ph);
    error = EN_open(ph, "./example_controls_from_until.inp", DATA_PATH_RPT, DATA_PATH_OUT);
    BOOST_REQUIRE(error == 0);

    error = EN_getcount(ph, EN_CONTROLCOUNT, &controlCount);
    BOOST_REQUIRE(error == 0);
    BOOST_CHECK(controlCount == 2);

    error = EN_getcontrol(ph, 1, &type, &linkIndex, &setting, &nodeIndex, &level, &fromTime, &untilTime);
    BOOST_REQUIRE(error == 0);
    BOOST_CHECK(type == EN_HILEVEL);
    BOOST_CHECK(abs(level - 0.0) < 0.001);
    BOOST_CHECK(abs(setting - 0.123) < 0.001);
    BOOST_CHECK(abs(fromTime - 10.0 * 3600.0) < 0.001);
    BOOST_CHECK(abs(untilTime - 13.0 * 3600.0) < 0.001);

    error = EN_getcontrol(ph, 2, &type, &linkIndex, &setting, &nodeIndex, &level, &fromTime, &untilTime);
    BOOST_REQUIRE(error == 0);
    BOOST_CHECK(type == EN_HILEVEL);
    BOOST_CHECK(abs(level - 0.0) < 0.001);
    BOOST_CHECK(abs(setting - 1.0) < 0.001);
    BOOST_CHECK(abs(fromTime - 14.0 * 3600.0) < 0.001);
    BOOST_CHECK(abs(untilTime - 23.0 * 3600.0) < 0.001);

    error = EN_getlinkindex(ph, (char *)"335", &pumpIndex);
    BOOST_REQUIRE(error == 0);

    error = EN_openH(ph);
    BOOST_REQUIRE(error == 0);
    error = EN_initH(ph, 0);
    BOOST_REQUIRE(error == 0);
    do {
        error = EN_runH(ph, &t);
        BOOST_REQUIRE(error < 7);
        
        error = EN_getlinkvalue(ph, pumpIndex, EN_SETTING, &pumpSetting);
        BOOST_REQUIRE(error == 0);

        if (t / 3600 == 10 ||
            t / 3600 == 11 ||
            t / 3600 == 12 ||
            t / 3600 == 13)
        {
            BOOST_CHECK(abs(pumpSetting - 0.123) < 0.001);
        }
        else
        {
            BOOST_CHECK(abs(pumpSetting - 1.0) < 0.001);
        }

        error = EN_nextH(ph, &tstep);
        BOOST_REQUIRE(error == 0);
    } while (tstep > 0);

    error = EN_closeH(ph);
    BOOST_REQUIRE(error == 0);

    error = EN_close(ph);
    BOOST_REQUIRE(error == 0);
    EN_deleteproject(ph);
}

BOOST_AUTO_TEST_SUITE_END()
