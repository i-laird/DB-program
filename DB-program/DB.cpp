/*
 * file: DB.cpp
 * author: Ian Laird
 * course: CSI 3335
 * due date: November 5, 2018
 *
 * This file contains the implementation of the DB class.
 */

#include "DB.h"

const std::set<std::string> VALID_TYPES = { "INT", "FLOAT", "STRING", "URL", "DATE", "DATETIME" };
const std::string USER_QUIT = "back";
bool const PARAM = true, RESULT = false;
bool DB::validType(std::string * toCheck) {
	for (int i = 0; i < toCheck->size(); i++) {
		(*toCheck)[i] = toupper( (*toCheck)[i]);
	}
	return VALID_TYPES.find(*toCheck) != VALID_TYPES.end();
 }

void DB::enterExperiment(Session & session, std::istream & in, std::ostream & out) {
	std::string experimentNumber = DB::getExperimentNumberCheckExist(session, in, out, NO_EXISTS);
	//case if the user did not enter an ex
	if (experimentNumber.empty()) {
		return;
	}
	DB::promptForExperiment(session, in, out, experimentNumber);
	in.ignore(); //because using getline in next function //might remove later
	//read in the param METADATA for this experiment
	DB::promptForParamOrResult(PARAM, session, in, out, experimentNumber);
	//read in the result METADATA for this experiment
	DB::promptForParamOrResult(RESULT, session, in, out, experimentNumber);
}

void DB::enterRun(Session & session, std::string experimentNumber, std::istream & in, std::ostream & out) {
	bool state = false;
	std::string time = DB::promptForRun(session, in, out, experimentNumber, state);
	if (time == "") { //this means it was not successful
		return;
	}
	in.ignore();
	//read in the params for this run
	DB::promptForRunParamOrResult(PARAM, state, session, in, out, experimentNumber, time);
	//read in the results for this run
	DB::promptForRunParamOrResult(RESULT, state, session, in, out, experimentNumber, time);
}


void DB::printExperimentMetaData(Session & session, std::string experimentCount, std::ostream & out) {
	RowResult res;
	std::set<Experiment> * experiment = NULL;
	std::set<ParameterType> * params = NULL;
	std::set<ResultType> * results = NULL;

	std::string queryString1 = "SELECT experimentid, managerid, date_format(startdate, '%Y-%m-%d') as startdate, date_format(dataentrydate, '%Y-%m-%d') as dataentrydate FROM Experiment.Experiment WHERE experimentid = " + experimentCount + ";";
	//get the experiment data
	try {
		res = session.sql(queryString1).execute();
		experiment = Experiment::extract(&res);
	}
	catch (const mysqlx::Error &err)
	{
		std::cerr << err << std::endl;
		std::cerr << "Could not retrieve the experiment data" << std::endl;
		return;
	}

	params = DB::getExperimentParams(session, experimentCount);
	results = DB::getExperimentResults(session, experimentCount);

	//NULL means not successful
	if (!params || !results) {
		return;
	}
	
	//this case is probably never reached because of earlier testing
	if (experiment->begin() == experiment->end()) {
		out << "the indicated experiment does not exist\n";
		return;
	}
	
	//now print the data that has been collected

	out << "here are the results for experiment\n\n\n";
	out << "------------------Experiment Info--------------------------\n\n";
	for (Experiment t : *experiment) {
		t.print(out);
		out << '\n';
	}
	out << "------------------Parameter Info--------------------------\n\n";
	for (ParameterType  t : *params) {
		t.print(out);
		out << '\n';
	}
	out << "------------------resultInfo--------------------------\n\n";
	for (ResultType t : *results) {
		t.print(out);
		out << '\n';
	}
	delete experiment;
	delete params;
	delete results;
}

void DB::printRunData(Session & session, std::string experimentCount, std::istream & in, std::ostream & out) {
	int count = 1, selection = 0;
	try {
		std::set<Run> * runs = DB::getRunsofExperiment(session, experimentCount);
		if (runs->empty()) {
			out << "there seem to be no runs for the given experiment\n";
			delete runs;
			return;
		}
		//print these runs out
		out << "Here are all of the runs for " << experimentCount << ":\n\n\n";
		for (Run const & t : *runs) {
			//out << count++ << ")\n";
			t.print(out, count++);
		}
		//now give user options to view run details
		out << "  ----------------------------------------------------------------\n\n\n";
		out << "Enter an index to view more details about that run(0 to quit)\n";
		do {
			in >> selection;
			if (selection > 0 && selection <= runs->size()) {
				//print the detailed run indicated
				auto t = runs->begin();
				//increment the iterator to the desired element (start at one for user convenience)
				for (int i = 1; i < selection; i++) {
					t++;
				}
				t->printDetailed(out);
				out << "Enter 0 to quit\n";
			}
			else if ( selection != 0) {
				out << "error not a valid selection...plese enter a selection between 1 & " << count - 1 << std::endl;
			}
		} while (selection != 0);
		delete runs;
	}
	catch (const mysqlx::Error &err)
	{
		std::cerr << err << std::endl;
		std::cerr << "the runs could not be retrieved" << std::endl;
		return;
	}
}

void DB::experimentReport(Session & session, std::string experimentNumber) {
	//first open output stream to the file that the html is to be written to
	std::string fileName = "ReportData_";
	fileName += experimentNumber;
	fileName += ".html";
	std::ofstream fout(fileName.c_str());
	if (fout.is_open()) {
		//Now get the runs
		std::set<Run> * runs = DB::getRunsofExperiment(session, experimentNumber);
		//Now print the results to the file
		DB::printToHTML(runs, fout, experimentNumber);
		fout.close(); //always close the file
		std::cout << "An HTML report has been generated with the name " << fileName << std::endl;
		delete runs;
	}
	else {
		std::cerr << "could not open file \n";
	}
}

void DB::displayAggregate(Session & session, std::string experimentNumber, std::istream & in,
	std::ostream & out) {

	std::set<ResultType> * results = NULL;
	std::set<std::string> validTypes;
	RowResult res;
	std::string readString;
	int count;
	double sum;

	std::string queryString = "SELECT * FROM Experiment.ResultTypes WHERE experimentid = " +
		experimentNumber + ";";

	//get the paramters for the experiment
	res = session.sql(queryString).execute();
	results = ResultType::extract(&res);
	//show which ones can be aggregated
	out << "Here are the results that can be aggregated:\n";
	for (auto const & t : *results) {
		std::string type = t.type;
		for (char & c : type) {
			c = tolower(c);
		}
		if (type == "float" || type == "int") {
			out << t.resultName << '\n';
			validTypes.insert(t.resultName);
		}
	}

	//get the date range
	out << "Please enter the lower bound of the date to be considered(not inclusive):\n";
	std::string lowerBound = DB::getValidTimeDate(in, out);

	out << "Please enter the upper bound of the date to be considered(not inclusive):\n";
	std::string upperBound = DB::getValidTimeDate(in, out);

	//get all of the run data
	std::set<Run> * runs = DB::getRunsofExperiment(session, experimentNumber, lowerBound, upperBound);

	out << "\nenter what result you would like aggregated(enter nothing when done)\n";
	in.ignore();
	for (;;) {
		count = 0;
		sum = 0;
		std::getline(in, readString);
		out << readString << std::endl;
		if (readString.empty()) {
			break;
		}
		if (validTypes.find(readString) == validTypes.end()) {
			out << "invalid selection. Please try again" << std::endl;
			continue;
		}
		//perform the aggregation
		for (Run const & r : *runs) {
			//check every run for the value
			if (r.success) {
				std::vector<RunResult>::const_iterator it = r.results->begin();
				while (it != r.results->end()) {
					if (it->resultName == readString) {
						std::istringstream converter(it->value);
						double temp;
						converter >> temp;
						if (converter) {
							count++;
							sum += temp;
						}
					}
					it++;
				}
			}
		}
		out << "the sum of " << readString << " was: " << sum << '\n';
		//make sure we do not get a divide by zero error
		if (sum > 0) {
			out << "the average for " << readString << " was: " << sum / count << '\n';
		}
		else {
			out << "the average is not calculable\n";
		}
	}
}

std::string DB::getExperimentNumberCheckExist(Session & session, std::istream & in, std::ostream & out, bool doesExist) {
	std::string experimentNumber;
	std::regex numRegex("[0-9]+");
	bool repeat = false;
	out << "Enter experiment number (enter " << USER_QUIT << " to go back):\n";
	std::string queryString;
	//get experiment that is valid and does not already exist
	do {
		repeat = false;
		in >> experimentNumber;
		if (experimentNumber == USER_QUIT) {
			return "";
		}
		queryString = "SELECT * FROM Experiment.Experiment WHERE experimentid = " +
			experimentNumber + ";";
		if (!std::regex_match(experimentNumber, numRegex)) {
			out << "sorry invalid experiment number...make sure it is only digits 0-9" << std::endl;
			repeat = true;
		}
		else if (doesExist == NO_EXISTS && session.sql(queryString).execute().count() > 0) {
			out << "looks like that experiment already exists" << std::endl;
			repeat = true;
		}
		else if (doesExist == EXISTS && session.sql(queryString).execute().count() == 0) {
			out << "looks like that experiment does not exist" << std::endl;
			repeat = true;
		}
	} while (repeat);
	return experimentNumber;
}


void DB::promptForExperiment(Session & session, std::istream & in, std::ostream & out, std::string const & experimentNumber) {
	std::string managerNumber, startDate, currentDate;
	std::string insertExperiment = "INSERT INTO experiment.experiment (experimentid, managerid, startdate,"
		"dataentrydate) VALUES (?,?,?,?);";
	std::regex managerRegex("[0-9]{6}");

	out << "Enter 6 digit Manager Number:\n";
	bool flagger = false;
	//get valid manager number
	while (!flagger) {
		flagger = true;
		in >> managerNumber;
		if (!std::regex_match(managerNumber, managerRegex)) {
			flagger = false;
			out << "invalid please try again...\n";
		}
	}
	bool flag = false;
	out << "enter start date (yyyy-mm-dd)\n";
	while (!flag) {
		flag = true;
		in >> startDate;
		if (!DB::checkType("date", startDate)) {
			flag = false;
			out << startDate << " is not of form yyyy-mm-dd" << std::endl;
		}
	}
	//DB::addSingleQuotesToString(startDate);
	flag = false;
	out << "enter current date (yyyy-mm-dd)\n";
	while (!flag) {
		flag = true;
		in >> currentDate;
		if (!DB::checkType("date", currentDate)) {
			flag = false;
			out << currentDate << " is not of of form yyyy-mm-dd'" << std::endl;
		}
	}
	//DB::addSingleQuotesToString(currentDate);

	//string insertQuery = "INSERT INTO experiment.experiment (experimentid, managerid, startdate,"
	//					"dataentrydate) VALUES (" + experimentNumber + ", " + managerNumber +
	//					", " + startDate + ", " + currentDate + ");";
	try {
		session.sql(insertExperiment).bind(experimentNumber).bind(managerNumber).bind(startDate).bind(currentDate).execute();
	}
	catch (const mysqlx::Error &err) {
		out << "ERR: " << err << std::endl;
		out << "the insertion of the experiment failed\nnow returning to main prompt\n" << std::endl;
		return;
	}
}

std::string DB::promptForRun(Session & session, std::istream & in, std::ostream & out, std::string const & experimentNumber, bool & s) {
	std::string personNumber, both;
	std::string success;
	std::string insertRun = "INSERT INTO experiment.Runs (experimentid, timeofrun, experimenterssn,"
		"success) VALUES (?,?,?,?);";
	std::regex personRegex("[0-9]{6}");

	out << "Enter 6 digit experimenter Number:\n";
	//get valid number
	do {
		in >> personNumber;
	} while (!std::regex_match(personNumber, personRegex));
	out << "Now entering the TIMEDATE of the run\n";
	both = DB::getValidTimeDate(in, out);

	/*
	bool flag = false;
	std::string time, date;

	while (!flag) {
		flag = true;
		out << "enter the time of the experiment(HH:mm:ss)\n";
		in >> time;
		out << "enter the date of the experiment(yyyy-mm-dd)\n";
		in >> date;
		both = date + " " + time;
		if (!DB::checkType("datetime", both)) {
			flag = false;
			out << both << " is not of of the form yyyy-mm-dd HH:mm:ss" << std::endl;
		}
	}
	*/
	//DB::addSingleQuotesToString(both);

	out << "enter if there was success (T/F)\n";
	in >> success;
	//make sure that it is a valid boolean char
	while (success.size() != 1 && success != "T" && success != "t" && success != "F" && success != "F") {
		out << "that is not a valid selection (enter T or F)\n";
		in >> success;
	}
	success[0] = tolower(success[0]);
	bool runSuccess = (success[0] == 't' ? true : false);
	s = runSuccess;
	try {
		std::cout << time << std::endl;
		session.sql(insertRun).bind(experimentNumber).bind(both).bind(personNumber).bind(runSuccess).execute();
		return both;
	}
	catch (const mysqlx::Error &err) {
		out << "ERR: " << err << std::endl;
		out << "the insertion of the Run failed\nnow returning to main prompt\n" << std::endl;
		return "";
	}
}

void DB::promptForParamOrResult(bool isParam, Session & session, std::istream & in, std::ostream & out, std::string const & experimentNumber) {
	//need 6 binds
	std::string insertQueryParam = "INSERT INTO experiment.parameterstypes (experimentid, parametername, type, required)"
		"VALUES (?,?,?,?)";
	std::string insertQueryResult = "INSERT INTO experiment.resulttypes (experimentid, resultname, type, required)"
		"VALUES (?,?,?,?)";
	out << "now enter the " << (isParam ? "params" : "results") << " for experiment " << experimentNumber;
	out << "\n-------------------------------------------------------------------------" << std::endl;;
	int count = 0;
	std::string name, typeOfThing;
	for (;;) {
		out << "now entering data for " << (isParam ? "param " : "result ") << count << ':' << std::endl;
		out << "enter the name (enter nothing to proceed)" << std::endl;
		std::getline(in, name);
		if (name == "") {
			return;
		}
		out << "enter the type of the " << (isParam ? "param" : "result") << " allowed types are INT, FLOAT, STRING, URL, DATE, and DATETIME)\n";
		std::getline(in, typeOfThing);
		while (!DB::validType(&typeOfThing)) {
			out << "sorry " << typeOfThing << "is not a valid type. The valid types are INT, FLOAT, STRING, URL, DATE, and DATETIME\n";
			std::getline(in, typeOfThing);
		}
		out << "enter if the " << (isParam ? "param" : "result") << " is required for every run of the experiment (T/F)" << std::endl;
		std::string required;
		bool isRequired;
		std::getline(in, required);
		//make sure that it is a valid boolean char
		while (required.size() != 1 && required != "T" && required != "t" && required != "F" && required != "F") {
			out << "that is not a valid selection (enter T or F)\n";
			std::getline(in, required);
		}
		//make sure it is either 'T' or 'F'
		required = toupper(required[0]);
		isRequired = (required == "T" ? true : false);
		string exper = experimentNumber;
		try {
			if (isParam) {
				session.sql(insertQueryParam)
					.bind(exper)
					.bind(name)
					.bind(typeOfThing)
					.bind(isRequired)
					.execute();
			}
			else {
				session.sql(insertQueryResult)
					.bind(exper)
					.bind(name)
					.bind(typeOfThing)
					.bind(isRequired)
					.execute();
			}
		}
		catch (const mysqlx::Error &err) {
			out << "ERR: " << err << std::endl;
			out << "the insertion of the" << (isParam ? "param" : "result") << "failed\n" << std::endl;
		}
	}
}

void DB::promptForRunParamOrResult(bool isParam, bool IsSuccess, Session & session, std::istream & in, std::ostream & out,
	std::string experimentNum, std::string experimentTime) {
	std::set<ResultType> * results = NULL;
	std::set<ParameterType> * params = NULL;
	std::map<std::string, std::string> names, mustEnter;
	if (isParam == PARAM) {
		params = DB::getExperimentParams(session, experimentNum);
	}
	else if (isParam == RESULT) {
		results = DB::getExperimentResults(session, experimentNum);
	}

	//need 4 binds
	std::string insertQueryParam = "INSERT INTO experiment.RunsParameter (experimentid, timeofrun, parametername, value)"
		"VALUES (?,?,?,?)";
	std::string insertQueryResult = "INSERT INTO experiment.runsResult (experimentid, timeofrun, resultname, value)"
		"VALUES (?,?,?,?)";
	out << "now enter the " << (isParam ? "params" : "results") << " for run at time " << experimentTime << std::endl;
	int count = 0;
	out << "Here are the " << (isParam ? "params" : "results") << " that may be entered\n";
	if (isParam == PARAM) {
		for (auto const & t : *params) {
			out << t.parameterName << " " << t.type << (t.required ? " REQUIRED" : " NOT REQUIRED") << '\n';
			names.insert(std::make_pair(t.parameterName, t.type));
			if (t.required) {
				mustEnter.insert(std::make_pair(t.parameterName, t.type));
			}
		}
	}
	else {
		for (auto const & r : *results) {
			out << r.resultName << " " << r.type << (r.required ? " REQUIRED" : " NOT REQUIRED") << '\n';
			names.insert(std::make_pair(r.resultName, r.type));
			if (r.required) {
				mustEnter.insert(std::make_pair(r.resultName, r.type));
			}
		}
	}
	//dont need to enter results on failure
	if (IsSuccess == false && isParam == RESULT) {
		mustEnter.clear();
	}
	std::string name, value;
	for (;;) {
		out << "enter the " << (isParam ? "param" : "result") << " name (enter nothing when done, REQ to see all still required, LEFT to see all)\n";
		std::getline(in, name);
		if (name.empty()) {
			if (mustEnter.empty()) {
				break;
			}
			else {
				out << "there are still required values to enter\n";
				if (IsSuccess == false && isParam == PARAM) {
					out << "even in failure params must be entered\n";
				}
			}
		}
		else if (name == "REQ") {
			out << "Here are the values still required:\n";
			for (auto t: mustEnter) {
				out << "     " << t.first <<  ' ' << t.second <<'\n';
			}
		}
		else if (name == "LEFT") {
			out << "Here are the values still able to be entered:\n";
			for (auto t : names) {
				out << "     " << t.first <<  ' ' << t.second << '\n';
			}
		}
		else if (names.find(name) != names.end()) {
			out << "enter the value of " << name << '\n';
			bool passed = false;
			while (!passed) {
				passed = true;
				std::getline(in, value);
				if (!DB::checkType(names[name], value)) {
					out << value << " is not a valid value...please try again\n";
					passed = false;
				}
			}
			names.erase(name);
			mustEnter.erase(name);
			try {
				if (isParam == PARAM) {
					session.sql(insertQueryParam)
						.bind(experimentNum)
						.bind(experimentTime)
						.bind(name)
						.bind(value)
						.execute();
				}
				else {
					session.sql(insertQueryResult)
						.bind(experimentNum)
						.bind(experimentTime)
						.bind(name)
						.bind(value)
						.execute();
				}
			}
			catch (const mysqlx::Error &err) {
				out << "ERR: " << err << '\n';
				out << "the insertion of the" << (isParam ? "param" : "result") << "failed\n" << std::endl;
			}
		}
		else {
			out << "not allowed\n";
		}	
	}
}

std::set<Run> * DB::getRunsofExperiment(Session & session, std::string  experimentNum, std::string lowerbound, std::string upperbound) {
	RowResult res;
	std::set<Run> * runs = NULL;

	std::string queryString = "SELECT experimentid, date_format(timeofrun, '%Y-%m-%d %H:%i:%s') as timeofrun, experimenterssn, success FROM Experiment.Runs WHERE experimentid = " +
		experimentNum;
	if (lowerbound != "") {
		DB::addSingleQuotesToString(lowerbound);
		DB::addSingleQuotesToString(upperbound);
		queryString += " AND timeofrun < ";
		queryString += upperbound;
		queryString += " AND timeofrun > ";
		queryString += lowerbound;
	}
	queryString += ";";

	//get all of the runs associated with a certain experiment
	res = session.sql(queryString).execute();
	//runs = Run::extract(&res); 
	//i modified the code and should use the later function which is a wrapper for extract
	runs = Run::getFullRun(session, &res);
	return runs;
}

void DB::printToHTML(std::set<Run> * runs, std::ostream & out, std::string experimentNum) {
	out << "<!DOCTYPE html>\n";
	out << "<html>\n";
	out << "<head>\n<style>\ntable,td,th{\n    border: 1px solid black;\n}\n</style>\n</head>";
	out << "<body>\n";
	out << "<h1> Experiment " << experimentNum << " Runs</h1>\n";
	out << "<table>\n";
	out << "  <tr>\n";
	out << "       <th>index</th>\n";
	out << "       <th>time of run</th>\n";
	out << "       <th>experimenter ssn</th>\n";
	out << "       <th>success</th>\n";
	out << "  </tr>\n";
	int count = 0;
	for (auto const & r : *runs) {
		out << "  <tr>\n";
		out << "      <td> " << ++count << "</td>\n";
		out << "      <td> " << r.timeOfRun << "</td>\n";
		out << "      <td> " << r.experimenterSSN << "</td>\n";
		out << "      <td> " << (r.success ? "true" : "false") << "</td>\n";
		out << "  </tr>\n";
	}
	out << "</table>" << std::endl;
	//Now print 2 tables for each result
	count = 0;
	for (auto const & r : *runs) {
		++count;
		r.printDetailedHTML(out, count);
	}
	out << "</body>\n";
	out << "</html>\n";
}

std::string DB::validParamOrResultName(bool isParam, Session & session, std::string experimentNum, std::string userInputString) {
	std::string queryStringParam = "SELECT * FROM Experiment.parameterstypes WHERE experimentid = ?;";
	std::string queryStringResult = "SELECT * FROM Experiment.resulttypes WHERE experimentid = ?;";

	std::set<ParameterType> * params = NULL;
	std::set<ResultType> * results = NULL;
	RowResult rs;
	//see if the indicated user string exists and if so return the type it must be
	if (isParam == PARAM) {
		rs = session.sql(queryStringParam).bind("experimentNum").execute();
		params = ParameterType::extract(&rs);
		for (auto const & p : *params) {
			if (p.parameterName == userInputString) {
				return p.type;
			}
		}
	}
	else if (isParam == RESULT) {
		rs = session.sql(queryStringResult).bind("experimentNum").execute();
		results = ResultType::extract(&rs);
		for (auto const & r : *results) {
			if (r.resultName == userInputString) {
				return r.type;
			}
		}
	}
	return "";
}

bool DB::checkType(std::string type, std::string & input) {
	bool flag = false;
	std::istringstream floatTest(input);
	//std::regex timeDate("(1?[0-9])|(2[0-4]):([0-5][0-9]|0)::((0?1)|(1[0-2]))/(0[1-9]|[1,2][0-9]|3[0,1])/[0-9]{1,4}");
	//std::regex date("(0?1)|(1[0-2])/(0[1-9]|[1,2][0-9]|3[0,1])/[0-9]{4}");
	std::regex date("[0-9]{4}-((0?[1-9])|(1[0-2]))-((0[1-9])|([1,2][0-9])|(3[0,1]))");
	std::regex timeDate("[0-9]{4}-((0?[1-9])|(1[0-2]))-((0[1-9])|([1,2][0-9])|(3[0,1])) ((1?[0-9])|(2[0-4])):(([0-5][0-9])|0):(([0-5][0-9])|0)");
	for (char & c : type)
		c = tolower(c);
	//see if the value is a valid floating point value
	if (type == "float") {
		float toStore;
		floatTest >> toStore;
		return floatTest && floatTest.eof();
	}
	if (type == "int") {
		int toStore;
		floatTest >> toStore;
		return floatTest && floatTest.eof();
	}
	if (type == "date") {
		if (!std::regex_match(input, date))
			flag = true;
	}
	if (type == "datetime") {
		if (!std::regex_match(input, timeDate))
			flag = true;
	}
	if (flag == true) {
		return false;
	}
//	DB::addSingleQuotesToString(input);
	return true; //maybe test url later
}

std::set<ParameterType> * DB::getExperimentParams(Session & session, std::string experimentCount) {
	std::string queryString2 = "SELECT * FROM Experiment.ParametersTypes WHERE experimentid = " +
		experimentCount + ";";
	std::set<ParameterType> * toReturn;
	RowResult res;
	try {
		//get the paramters for the experiment
		res = session.sql(queryString2).execute();
		toReturn = ParameterType::extract(&res);
		return toReturn;
	}
	catch (const mysqlx::Error &err)
	{
		std::cerr << err << std::endl;
		std::cerr << "Could not retrieve the parameters" << std::endl;
		return NULL;
	}
}
std::set<ResultType> * DB::getExperimentResults(Session & session, std::string experimentCount) {
	std::string queryString3 = "SELECT * FROM Experiment.ResultTypes WHERE experimentid = " +
		experimentCount + ";";
	RowResult res;
	std::set<ResultType> * toReturn = NULL;
	try {
		//get the results for the experiment
		res = session.sql(queryString3).execute();
		toReturn = ResultType::extract(&res);
		return toReturn;
	}
	catch (const mysqlx::Error &err)
	{
		std::cerr << err << std::endl;
		std::cerr << "Could not retrieve the results" << std::endl;
		return NULL;
	}
}
 std::string DB::getValidTimeDate(std::istream & in, std::ostream & out) {
	 std::string time, date, both;
	 bool flag = false;

	 while (!flag) {
		 flag = true;
		 out << "enter the time(HH:mm:ss)\n";
		 in >> time;
		 out << "enter the date(yyyy-mm-dd)\n";
		 in >> date;
		 both = date + " " + time;
		 if (!DB::checkType("datetime", both)) {
			 flag = false;
			 out << both << " is not of of the form yyyy-mm-dd HH:mm:ss" << std::endl;
		 }
	 }
	 return both;
}

