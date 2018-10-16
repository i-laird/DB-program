/*
 * file: ParameterType.h
 * author: Ian Laird
 * course: CSI 3335
 * due date: November 5, 2018
 *
 * This file contains the implementation of the DB parameterType class.
 */
#include "ParameterType.h"


std::set< ParameterType > * ParameterType::extract(mysqlx::RowResult * rs) {
	std::set<ParameterType> * toReturn = new std::set< ParameterType>;
	Row r;
	while (r = rs->fetchOne()) {
		ParameterType toInsert;
		for (unsigned i = 0; i < rs->getColumnCount(); i++) {
			std::string columnName = rs->getColumn(i).getColumnName();
			std::transform(columnName.begin(), columnName.end(), columnName.begin(), ::tolower);
			if (columnName == "experimentid") {
				std::stringstream converter;
				converter << r[i];
				converter >> toInsert.experimentId;
			}
			else if (columnName == "parametername") {
				toInsert.parameterName = r[i];
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

void ParameterType::print(std::ostream & out) {
	out << std::setw(18) << "experiment id" <<  ':' << this->experimentId << std::endl;
	out << std::setw(18) << "parameter name" << ':' << this->parameterName << std::endl;
	out << std::setw(18) << "type" << ':' << this->type << std::endl;
	out << std::setw(18) << "required" << ':' << (this->required ? "true" : "false") << std::endl;
}
