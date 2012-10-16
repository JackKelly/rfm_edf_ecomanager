/*
 * DynamicArray_test.cpp
 *
 *  Created on: 16 Oct 2012
 *      Author: jack
 */

#include <iostream>

#define TESTING

#include "../DynamicArray.h"
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE DynamicArrayTest
#include <boost/test/unit_test.hpp>

DynamicArray<int> setup()
{
    uint8_t length = 20;
    DynamicArray<int> da;

    std::cout << "Appending " << (int)length << " ints..." << std::endl;
    for (uint8_t i=0; i<length; i++) {
        da.append(i*5);
    }

    return da;
}

BOOST_AUTO_TEST_CASE(printAndAppend)
{
    std::cout << "Testing DynamicArray..." << std::endl;

    DynamicArray<int> da = setup();

    da.print();

    int first_array[] = {0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 95};

    for (uint8_t i=0; i<20; i++) {
        BOOST_CHECK(da[i] == first_array[i]);
    }

    std::cout << "Appending 41..." << std::endl;

    da.append(41);
    da.print();

    int second_array[] = {0, 5, 10, 15, 20, 25, 30, 35, 40, 41, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 95};

    for (uint8_t i=0; i<21; i++) {
        BOOST_CHECK(da[i] == second_array[i]);
    }

}

BOOST_AUTO_TEST_CASE(find)
{
    std::cout << "Testing find()..." << std::endl;
    DynamicArray<int> da = setup();

    uint8_t index;

    std::cout << "Testing find()" << std::endl;
    BOOST_CHECK( da.find(30, &index) );
    BOOST_CHECK(index == 6);

}
