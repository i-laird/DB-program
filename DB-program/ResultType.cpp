/*
 * file: ResultType.cpp
 * author: Ian Laird
 * course: CSI 3335
 * due date: November 5, 2018
 *
 * This file contains the implementation of the ResultType class.
 */
#include "ResultType.h"

std::set< ResultType > * ResultType::extract(mysqlx::RowResult * rs) {
	std::set<ResultType> * toReturn = new std::set< ResultType>;
	Row r;
	while (r = rs->fetchOne()) {
		ResultType toInsert;
		for (unsigned i = 0; i < rs->getColumnCount(); i++) {
			std::string columnName = rs->getColumn(i).getColumnName();
			std::transform(columnName.begin(), columnName.end(), columnName.begin(), ::tolower);
			if (columnName == "experimentid") {
				std::stringstream converter;
				converter << r[i];
				converter >> toInsert.experimentId;
			}
			else if (columnName == "resultname") {
				toInsert.resultName = r[i];
			}
			else if (columnName == "type") {
				toInsert.type = r[i];
			}
			else if (columnName == "required") {
				toInsert.required = r[i];
			}
			else
			{
				//TODO exception
			}
		}
		toReturn->insert(toInsert);
	}
	return toReturn;
}

void ResultType::print(std::ostream & out) const{
	out << std::setw(18) << "experiment id" << ':' << this->experimentId << std::endl;
	out << std::setw(18) <<"result name" << ':' << this->resultName << std::endl;
	out << std::setw(18) << "type" << ':' << this->type << std::endl;
	out << std::setw(18) << "required" << ':' << (this->required ? "true" : "false") << std::endl;
}