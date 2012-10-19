/*
 * DynamicArray_test.cpp
 *
 *  Created on: 16 Oct 2012
 *      Author: jack
 */

#include <iostream>

#include "../RollingAv.h"
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE RollingAvTest
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(rollingAv)
{
    RollingAv ra;
    BOOST_CHECK( ra.get_av() == 6000 );

    ra.add_sample(6100);
    BOOST_CHECK( ra.get_av() == 6020 );

    ra.add_sample(6400);
    BOOST_CHECK( ra.get_av() == 6100 );

    ra.add_sample(6150);
    BOOST_CHECK( ra.get_av() == 6130 );

    ra.add_sample(6150);
    BOOST_CHECK( ra.get_av() == 6160 );

    ra.add_sample(5999);
    BOOST_CHECK( ra.get_av() == 6159 );

    ra.add_sample(6231);
    BOOST_CHECK( ra.get_av() == 6186 );

}

