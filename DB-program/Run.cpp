/*
 * file: Run.h
 * author: Ian Laird
 * course: CSI 3335
 * due date: November 5, 2018
 *
 * This file contains the implementation of the Run class.
 */
#include "Run.h"
#include <vector>
#include <iostream>

Run::Run(Run const & other) {
	this->experimentId = other.experimentId;
	this->timeOfRun = other.timeOfRun;
	this->success = other.success;
	this->experimenterSSN = other.experimenterSSN;
	this->params = (other.params ? new std::vector<RunParam>(*other.params) : NULL);
	this->results = (other.results ? new std::vector<RunResult>(*other.results) : NULL);
}
std::set< Run > * Run::extract(mysqlx::RowResult * rs) {
	std::set<Run> * toReturn = new std::set< Run>;
	Row r;
	while (r = rs->fetchOne()) {
		Run toInsert;
		for (unsigned i = 0; i < rs->getColumnCount(); i++) {
			std::string columnName = rs->getColumn(i).getColumnLabel();
			std::transform(columnName.begin(), columnName.end(), columnName.begin(), ::tolower);
			if (columnName == "experimentid") {
				std::stringstream converter;
				converter << r[i];
				converter >> toInsert.experimentId;
			}
			else if (columnName == "timeofrun") {
				toInsert.timeOfRun = r[i];
			}
			else if (columnName == "experimenterssn") {
				std::stringstream converter;
				converter << r[i];
				converter >> toInsert.experimenterSSN;
			}
			else if (columnName == "success") {
				toInsert.success = r[i];
			}
			else
			{
				//TODO exception
			}
		}
		toReturn->insert(toInsert);
	}
	//std::set<int> Test;
	return toReturn;
}

std::set< Run > * Run::getFullRun(Session & session, RowResult * rs) {
	std::set< Run > * toReturn = Run::extract(rs);
	Run::addParamsAndResults(toReturn, session);
	return toReturn;
}

void Run::addParamsAndResults(std::set< Run > * toModify, Session & session) {
	std::string queryStringParam = "SELECT parametername,value FROM Experiment.RunsParameter WHERE experimentid = ? AND timeofrun = ?;";
	std::string queryStringResult = "SELECT resultname,value FROM Experiment.RunsResult WHERE experimentid = ? AND timeofrun = ?;";

	RowResult res, res2;
	for (Run const & r : *toModify) {
		res = session.sql(queryStringParam).bind(r.experimentId).bind(r.timeOfRun).execute();
		res2 = session.sql(queryStringResult).bind(r.experimentId).bind(r.timeOfRun).execute();
		r.setParam(RunParam::extract(&res));
		r.setresult(RunResult::extract(&res2));
	}
}

void Run::print(std::ostream & out, int count) const{
	if (count >= 0) {
		out << count;
		out << "-------------------------------------------------------------\n";
	}
	out << "     experiment id : " << this->experimentId << std::endl;
	out << "     time of the run : " << this->timeOfRun << std::endl;
	out << "     experimenter ssn : " << this->experimenterSSN << std::endl;
	out << "     success :  " << (this->success ? "true" : "false") << std::endl;
}

void Run::printDetailed(std::ostream & out) const {
	out << "----------------------------------------------------------------------\n";
	this->print(out, -1);
	int count = 1;

	out << "\nPARAMS:\n";
	if (this->params) {
		for (auto & p : *this->params) {
			out << count << ")\n";
			out << "     Name: " << p.parameterName << std::endl;
			out << "     Value :" << p.value << std::endl;
			++count;
		}
	}
	count = 1;
	out << "\nRESULTS:\n";
	if (this->results) {
		for (auto & r : *this->results) {
			out << count << ")\n";
			out << "     Name:" << r.resultName << std::endl;
			out << "     Value:" << r.value << std::endl;
			++count;
		}
	}
	out << "\n----------------------------------------------------------------------\n";

}

void Run::printDetailedHTML(std::ostream & out, int count) const{
	if (this->params) {
		out << "<h2>Params for index " << count << "</h2>\n";
		out << "<table>\n";
		out << "  <tr>\n    <th>Parameter Name</th>\n    <th>value</th>\n  </tr>";
			for (auto & p : *this->params) {
				out << "  <tr>\n";
				out << "     <td> " << p.parameterName << "</td>\n";
				out << "     <td> " << p.value << "</td>\n";
				out << "  </tr>\n";
			}
		out << "</table>" << std::endl;
	}
	if (this->results) {
		out << "<h2>Results for index " << count << "</h2>\n";
		out << "<table>\n";
		out << "  <tr>\n    <th>result Name</th>\n    <th>value</th>\n  </tr>";
		for (auto & r : *this->results) {
			out << "  <tr>\n";
			out << "    <td> " << r.resultName << "</td>\n";
			out << "    <td> " << r.value << "</td>\n";
			out << "  </tr>\n";
		}
		out << "</table>" << std::endl;
	}
}

