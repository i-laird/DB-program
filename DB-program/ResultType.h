/*
 * file: ResultType.h
 * author: Ian Laird
 * course: CSI 3335
 * due date: November 5, 2018
 *
 * This file contains the implementation of the ResultType class
 */
#pragma once
#include <string>
#include <mysqlx/xdevapi.h>
#include <set>
#include <iomanip>
using namespace mysqlx;

class ResultType
{
public:
	long experimentId;
	std::string resultName;
	std::string type;
	bool required;
	/*
	 * extract
	 *
	 * This function extracts Resulttypes from a rowresult.
	 *
	 * Parameters:
	 *   rs: the results of an SQL query
	 *
	 * Return value: a set of all ResultTypes in the given RowResult
	 */
	static std::set< ResultType > * extract(RowResult * rs);
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
	void print(ostream & out) const;
	const bool operator<(ResultType const & other) const {
		return this->resultName < other.resultName;
	}
};

