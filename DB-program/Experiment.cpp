/*
 * file: Run.h
 * author: Ian Laird
 * course: CSI 3335
 * due date: November 5, 2018
 *
 * This file contains the implementation of the Experiment class.
 */
#include "Experiment.h"
#include <sstream>
#include <iostream>

std::set<Experiment> * Experiment::extract(RowResult * rs) {
	std::set<Experiment> * toReturn = new std::set< Experiment>;
	Row r;
	while (r = rs->fetchOne()) {
		Experiment toInsert;
		for (unsigned i = 0; i < rs->getColumnCount(); i++) {
			std::string columnName = rs->getColumn(i).getColumnLabel();
			std::transform(columnName.begin(), columnName.end(), columnName.begin(), ::tolower);
			if (columnName == "experimentid") {
				std::stringstream converter;
				converter << r[i];
				converter >> toInsert.experimentId;
			}
			else if (columnName == "managerid") {
				std::stringstream converter;
				converter << r[i];
				converter >> toInsert.managerId;
			}
			else if (columnName == "startdate") {
				toInsert.startDate = r[i];
			}
			else if (columnName == "dataentrydate") {
				toInsert.dataEntryDate = r[i];
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

void Experiment::print(std::ostream & out) {
	out << std::setw(18) << "experiment id" << ':' << this->experimentId << std::endl;
	out << std::setw(18) << "manager id" << ':' << this->managerId << std::endl;
	out << std::setw(18) << "startdate" << ':' << this->startDate << std::endl;
	out << std::setw(18) << "data entry date" << ':' << this->dataEntryDate << std::endl;
}