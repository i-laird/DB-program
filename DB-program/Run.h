/*
 * file: Run.h
 * author: Ian Laird
 * course: CSI 3335
 * due date: November 5, 2018
 *
 * This file contains the declaration of the Run class.
 */
#pragma once
#include <string>
#include <mysqlx/xdevapi.h>
#include "RunResult.h"
#include "RunsParam.h"
#include <set>
#include <vector>
using namespace mysqlx;

class Run
{
public:
	long experimentId;
	std::string timeOfRun;
	long experimenterSSN;
	bool success;
	mutable std::vector<RunParam> * params;
	mutable std::vector<RunResult> * results;
	Run & operator=(Run const & other) {
		if (this != &other) {
			delete params;
			delete results;

			this->experimentId = other.experimentId;
			this->timeOfRun = other.timeOfRun;
			this->success = other.success;
			this->experimenterSSN = other.experimenterSSN;
			this->params = new std::vector<RunParam>(*other.params);
			this->results = new std::vector<RunResult>(*other.results);
		}
		return *this;
	}
	Run(Run const & other);
	Run() {
		params = NULL;
		results = NULL;
	}
	~Run() {
		delete params;
		delete results;
	}

	static std::set< Run > * extract(RowResult * rs);
	static void addParamsAndResults(std::set< Run > * toModify, Session & session);
	static std::set< Run > * getFullRun(Session & session, RowResult * rs);
	void print(std::ostream & out, int count) const;
	const bool operator<(Run const & other) const {
		return this->timeOfRun < other.timeOfRun;
		//return this->experimentId < other.experimentId;
	}
	void printDetailed(std::ostream & out) const;
	void printDetailedHTML(std::ostream & out, int count) const;
	void setParam(std::vector<RunParam> * params) const {
		this->params = params;
	}
	void setresult(std::vector<RunResult> * results) const {
		this->results = results;
	}
};