/*
 * file: RunsParam.cpp
 * author: Ian Laird
 * course: CSI 3335
 * due date: November 5, 2018
 *
 * This file contains the implementation of the RunsParam class.
 */
#include "RunsParam.h"

RunParam::RunParam()
{
}


RunParam::~RunParam()
{
}
std::vector<RunParam> * RunParam::extract(RowResult * rs) {
	std::vector<RunParam> * toReturn = new std::vector< RunParam>;
	Row r;
	while (r = rs->fetchOne()) {
		//std::stringstream converter;
		RunParam toInsert;
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
			if (columnName == "parametername") {
				toInsert.parameterName = r[i];
				//converter >> toInsert.parameterName;
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