/*
 * file: Experiment.h
 * author: Ian Laird
 * course: CSI 3335
 * due date: November 5, 2018
 *
 * This file contains the declaration of the Experiment class.
 */
#pragma once
#include <string>
#include <set>
#include <mysqlx/xdevapi.h>
#include <iomanip>

using namespace mysqlx;

struct Experiment
{
public:
	long experimentId;
	long managerId;
	std::string  startDate;
	std::string dataEntryDate;
	/*
	 * extract
	 *
	 * This function extracts Experiments from a rowresult.
	 *
	 * Parameters:
	 *   rs: the results of an SQL query
	 *
	 * Return value: a set of all Experiments in the given RowResult
	 */
	static std::set<Experiment> * extract(RowResult * rs);
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
	const bool operator<(Experiment const & other) const {
		return this->experimentId < other.experimentId;
	}
};

