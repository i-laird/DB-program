/*
 * file: RunResult.h
 * author: Ian Laird
 * course: CSI 3335
 * due date: November 5, 2018
 *
 * This file contains the declaration of the RunsParam class
 */
#pragma once
#include <vector>
#include <mysqlx/xdevapi.h>

using namespace mysqlx;

class RunResult
{
public:
	std::string resultName, value;
	RunResult();
	~RunResult();
	static std::vector<RunResult> * extract(RowResult * rs);
};

