/*
 * file: RunsParam.h
 * author: Ian Laird
 * course: CSI 3335
 * due date: November 5, 2018
 *
 * This file contains the implementation of the RunsParam class
 */
#pragma once
#include <vector>
#include <mysqlx/xdevapi.h>

using namespace mysqlx;

class RunParam
{
public:
	std::string parameterName, value;
	RunParam();
	~RunParam();
	static std::vector<RunParam> * extract(RowResult * rs);
};

