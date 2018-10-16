/*
 * file: RunResult.cpp
 * author: Ian Laird
 * course: CSI 3335
 * due date: November 5, 2018
 *
 * This file contains the implementation of the RunResult class.
 */
#include "RunResult.h"

RunResult::RunResult()
{
}


RunResult::~RunResult()
{
}

std::vector<RunResult> * RunResult::extract(RowResult * rs) {
	std::vector<RunResult> * toReturn = new std::vector< RunResult>;
	Row r;
	while (r = rs->fetchOne()) {
		//std::stringstream converter;
		RunResult toInsert;
		for (unsigned i = 0; i < rs->getColumnCount(); i++) {
			std::string columnName = rs->getColumn(i).getColumnName();
			std::transform(columnName.begin(), columnName.end(), columnName.begin(), ::tolower);
			//converter << r[i];
			/*
			if (columnName == "experimentid") {
				converter >> toInsert.experimentId;
			}
			else if (columnName == "timeofrun") {
				converter >> toInsert.timeOfRun;
			}
			*/
			if (columnName == "resultname") {
				//converter >> toInsert.resultName;
				toInsert.resultName = r[i];
			}
			else if (columnName == "value") {
				toInsert.value = r[i];
				//converter >> toInsert.value;
			}
			else
			{
				//TODO exception
			}
		}
		toReturn->push_back(toInsert);
	}
	return toReturn;
}
