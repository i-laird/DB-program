/*
 * file: Run.h
 * author: Ian Laird
 * course: CSI 3335
 * due date: November 5, 2018
 *
 * This file contains the implementation of the DB class.
 */
#pragma once
#include "Experiment.h"
#include "Run.h"
#include "ParameterType.h"
#include "ResultType.h"

#include <mysqlx/xdevapi.h>
#include <iostream>
#include <set>
#include <string>
#include <fstream>
#include <regex>
#include <vector>
#include <sstream>
#include <iostream>
#include <map>
using namespace mysqlx;
bool const EXISTS = true, NO_EXISTS = false;
/*
DB consists solely of static methods and is the way that the tasks are fulfilled for the assignment.
It consists of a function call for every one of the 6 requirments plus many helper functions.
*/
class DB {
public:
	/*
	 * enterExperiment
	 *
	 * This function allows the user to enter an experiment.
	 *
	 * Parameters:
	 *   session: the MYSQl session that has been created already
	 *   in: an input stream for user input
	 *   out: where ouput should be directed
	 *
	 * Return value: void
	 */
	static void enterExperiment(Session & session, std::istream & in, std::ostream & out);
	/*
	 * enterRun
	 *
	 * This function allows the user to enter a run of an experiment.
	 *
	 * Parameters:
	 *   session: the MYSQl session that has been created already
	 *   in: an input stream for user input
	 *   experimentNumber: the experiment number that the run is to be associated with
	 *
	 * Return value: void
	 */
	static void enterRun(Session & session, std::string experimentNumber, std::istream & in,
		std::ostream & out);
	/*
	 * printExperimentMetaData
	 *
	 * This function prints all METADATA about a requested experiment.
	 *
	 * Parameters:
	 *   session: the MYSQl session that has been created already
	 *   out: an stream where user ouput should be directed
	 *   experimentNumber: the experiment number to be printed
	 *
	 * Return value: void
	 */
	static void printExperimentMetaData(Session & session, std::string experimentNumber,
		std::ostream & out);
	/*
	 * printRunData
	 *
	 * This function prints all data about a requested run of an experiment.
	 *
	 * Parameters:
	 *   session: the MYSQl session that has been created already
	 *   out: an stream where user ouput should be directed
	 *   experimentNumber: the experiment number that the run is associated with
	 *   in: the input stream where user input comes from
	 *
	 * Return value: void
	 */
	static void printRunData(Session & session, std::string experimentNumber, std::istream & in,
		std::ostream & out);
	/*
	 * experimentReport
	 *
	 * This function generated an experiment report for the requested experiment report.
	 *     It generates an HTML document.
	 *
	 * Parameters:
	 *   session: the MYSQl session that has been created already
	 *   experimentNumber: the experiment number that the document should be summarizing.
	 *
	 * Return value: void
	 */
	static void experimentReport(Session & session, std::string experimentNumber);
	/*
	 * displayAggregate
	 *
	 * This function allows the user to aggregate results.
	 *
	 * Parameters:
	 *   session: the MYSQl session that has been created already
	 *   experimentNumber: the experiment number that the document should be summarizing.
     *   in: the input stream where user input comes from
	 *   out: the output stream to print to
	 *
	 * Return value: void
	 */
	static void displayAggregate(Session & session, std::string experimentNumber, std::istream & in,
		std::ostream & out);

	//helper functions///////////////////////////////////////////////////////////////////////////

		/*
	 * validType
	 *
	 * This function tests if a string is of a valid type(INT, FLOAT, URL, STRING, DATE, DATETIME)
	 *
	 * Parameters:
	 *   toCheck: the string to check for type validity
	 *
	 * Return value: TRUE means that toCheck is a valid type
	 */
	static bool validType(std::string * toCheck);
	/*
	* getExperimentNumberCheckExist
	*
	* This function gets a experimentNum from user and tests whether an experiment exists.
	*
	* Parameters:
	 *   session: the MYSQl session that has been created already
	 *   in: the input stream where user input comes from
	 *   out: the output stream to print to
	 *   doesExist: indicates whether or not the experimentNum should already exist in the database.
	*
	* Return value: an experimentNumber
	*/	
	static std::string getExperimentNumberCheckExist(Session & session, std::istream & in, 
		std::ostream & out, bool doesExist);
	//TODO maybe add this later?
	//static std::string getExperimentNumberNoCheck(std::istream & in, std::ostream & out);
	/*
	* promptForExperiment
	*
	* This function reads in and then saves METADATA about an experiment.
	*
	* Parameters:
	 *   session: the MYSQl session that has been created already
	 *   in: the input stream where user input comes from
	 *   out: the output stream to print to
	 *   experimentNum: the experimentNum that is to be associated with this experiment
	*
	* Return value: an experimentNumber
	*/
	static void promptForExperiment(Session & session, std::istream & in, std::ostream & out, 
		std::string const & experimentNum);
	/*
	* promptForRun
	*
	* This function reads in and then saves a run of an experiment.
	*
	* Parameters:
	 *   session: the MYSQl session that has been created already
	 *   in: the input stream where user input comes from
	 *   out: the output stream to print to
	 *   experimentNum: the experimentNum that is to be associated with this run
	 *   s: used to return if the run was successful
	*
	* Return value: "" indicates failure, else returns the DATETIME of the run
	*/
	static std::string promptForRun(Session & session, std::istream & in, std::ostream & out, std::string const & experimentNum, bool & s);
	/*
	* promptForParamOrResult
	*
	* This function reads in and then saves METADATA about either a parameter or a result.
	*
	* Parameters:
	 *   session: the MYSQl session that has been created already
	 *   in: the input stream where user input comes from
	 *   out: the output stream to print to
	 *   experimentNum: the experimentNum that is to be associated with this run
	*
	* Return value: void
	*/
	static void promptForParamOrResult(bool, Session & session, std::istream & in, std::ostream & out, 
		std::string const & experimentNum);
	/*
	* promptForRunParamOrResult
	*
	* This function reads in and then saves parameters or results of a run.
	*
	* Parameters:
	 *   session: the MYSQl session that has been created already
	 *   in: the input stream where user input comes from
	 *   out: the output stream to print to
	 *   experimentNum: the experimentNum that is to be associated with this run
	 * experimentTime: the DATETIME of the experiment
	*
	* Return value: void
	*/
	static void promptForRunParamOrResult(bool, bool, Session & session, std::istream & in, std::ostream & out, 
		std::string experimentNum, std::string experimentTime);
	/*
	* getRunsofExperiment
	*
	* This function gets all the runs of an experiment.
	*
	* Parameters:
	*   session: the MYSQl session that has been created already
	*   experimentNum: the experimentNum that is to be associated with this run
	*
	* Return value: a set of all runs of the indicated experiment.
	*/
	static std::set<Run> * getRunsofExperiment(Session & session, std::string  experimentNum, std::string whereClause = "", std::string upperbound = "");
	/*
	* printToHTML
	*
	* This function prints data about an experiment to a file in HTML.
	*
	* Parameters:
	*   runs: the runs to print to the file
	*   out: the file to be printed to
	*   experimentNum: the desired experiment
	*
	* Return value: void
	*/
	static void printToHTML(std::set<Run> * runs, std::ostream & out, std::string experimentNum);
	/*
	* validParamOrResultName
	*
	* This function sees if the param or result name is "valid" ie in METADATA.
	*
	* Parameters:
	*   session: the MYSQl session that has been created already
	*   experimentNum: the experimentNum that is to be associated with this run
	*   userInputString: the string to check
	*   isParam: indicates if a param or a result
	*
	* Return value: "" indicates invalid name. Else the required type of the param or result.
	*/
	static std::string validParamOrResultName(bool isParam, Session & session, std::string experimentNum, std::string userInputString);
	/*
	* checkType
	*
	* This function sees if a string is of a required type
	*
	* Parameters:
	*   type: the type desired of the string
	*   input: the string to check
	*
	* Return value: TRUE means input is of type type
	*/
	static bool checkType(std::string type, std::string & input);
	/*
	* addSingleQuotesToString
	*
	* This function adds single quotes to the beginnig and end of a string
	*
	* Parameters:
	*   toChange: the string to change
	*
	* Return value: void
	*/
	static void addSingleQuotesToString(std::string & toChange) {
		toChange.insert(0, "'");
		toChange += "'";
	}
	/*
	* getExperimentParams
	*
	* gets all params of an experiement (METADATA)
	*
	* Parameters:
	*   session: the MYSQL session object
	*   experimentCount: the num of the experiment
	*
	* Return value: set of all param types
	*/
	static std::set<ParameterType> * getExperimentParams(Session & session, std::string experimentCount);
	/*
	* getExperimentResults
	*
	* gets all results of an experiement (METADATA)
	*
	* Parameters:
	*   session: the MYSQL session object
	*   experimentCount: the num of the experiment
	*
	* Return value: set of all result types
	*/
	static std::set<ResultType> * getExperimentResults(Session & session, std::string experimentCount);

	static std::string getValidTimeDate(std::istream & in, std::ostream & out);
};


