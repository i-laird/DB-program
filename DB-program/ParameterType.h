/*
 * file: ParameterType.h
 * author: Ian Laird
 * course: CSI 3335
 * due date: November 5, 2018
 *
 * This file contains the implementation of the ParameterType class
 */
#pragma once
#include <string>
#include <mysqlx/xdevapi.h>
#include <set>
#include <iomanip>

using namespace mysqlx;

class ParameterType
{
public:
	long experimentId;
	std::string parameterName;
	std::string type;
	bool required;
	/*
	 * extract
	 *
	 * This function extracts ParameterTypes from a rowresult.
	 *
	 * Parameters:
	 *   rs: the results of an SQL query
	 *
	 * Return value: a set of all ParameterTypes in the given RowResult
	 */
	static std::set< ParameterType > * extract(RowResult * rs);
	/*
	 * print
	 *
	 * prints to output.
	 *
	 * Parameters:
	 *   out: the output stream to print to
	 *
	 * Return value: void
	 */
	void print(std::ostream & out);
	const bool operator<(ParameterType const & other) const {
		return this->parameterName < other.parameterName;
	}
};

