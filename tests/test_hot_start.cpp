/*
 ******************************************************************************
 Project:      OWA EPANET
 Version:      2.2
 Module:       test_hot_start.cpp
 Description:  Tests EPANET toolkit api functions
 Authors:      see AUTHORS
 Copyright:    see AUTHORS
 License:      see LICENSE
 Last Updated: 07/20/2019
 ******************************************************************************
*/

/*
   Tests the Pressure Driven Analysis option
*/

#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>

#include "test_toolkit.hpp"


BOOST_AUTO_TEST_SUITE (test_pda)

BOOST_AUTO_TEST_CASE(test_pda_model)

{
    int error = 0;
    int index;
    double count, reduction;

    EN_Project ph = NULL;
    error = EN_createproject(&ph);
    error = EN_open(ph, DATA_PATH_NET1, DATA_PATH_RPT, "");

    BOOST_REQUIRE(boost::filesystem::exists("state.out"));
}

BOOST_AUTO_TEST_SUITE_END()
