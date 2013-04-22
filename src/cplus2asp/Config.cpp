/* Generated by re2c 0.13.5 on Mon Apr 22 16:39:44 2013 */
#line 1 "Config.re2c"
/*
 * Copyright (c) 2010-2013 <Joseph Babb, Michael Cassollary, Joohyung Lee>
 *
 * For information on how to contact the authors, please visit
 *	http://reasoning.eas.asu.edu/cplus2asp
 *
 * This file is part of the cplus2asp system.
 *
 * cplus2asp is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * cplus2asp is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


/*
 * Config.cpp
 *
 *  Created on: Jan 23, 2013
 *      Author: Joseph Babb
 */

#include <cstring>
#include <cstdio>
#include <string>
#include <sstream>
#include <stdexcept>
#include <climits>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include "boost/process.hpp"

#include "Config.h"
#include "utils.h"

namespace fs = boost::filesystem;
namespace bp = boost::process;

/***************************************************************/
/* Constants */
/***************************************************************/
unsigned int const Config::UNDEFINED = UINT_MAX;
unsigned int const Config::INF = UINT_MAX - 4;
unsigned int const Config::STATES_QUERY = UINT_MAX - 3;
unsigned int const Config::TRANSITIONS_QUERY = UINT_MAX - 2;
unsigned int const Config::NO_QUERY = UINT_MAX - 1;

/***************************************************************/
/* Methods */
/***************************************************************/

// Default Constructor
Config::Config(std::string const& name) {

	fs::path temppath = fs::temp_directory_path();

	if (name != "") {
	
		try {
			mPath = fs::canonical(bp::search_path(name)).parent_path().native();
		} catch (fs::filesystem_error &e) {
			debug("Caught exception: '%s'", true, e.what());
			mPath = "";
		}
	} else {
		mPath = "";
	}

	mErrFile = (temppath /  fs::unique_path()).native();

	mRunningMode = DEF_RUNNING_MODE;
	mCustomRunningMode = 0;

	mLang = DEF_LANG;
	mCustomLang = 0;

	mNoneAlias = "";
	mCustomNone = 0;

	mSymTabFile = (temppath / fs::unique_path()).native();
	mCustomSymTab = 0;

	mConfigOpts[OPT_INCL_STD] = DEF_INCL_STD;
	mConfigOpts[OPT_INCL_ADDITIVE] = DEF_INCL_ADDITIVE;
	mConfigOpts[OPT_DISCARD_F2LP] = DEF_DISCARD_F2LP;
	mConfigOpts[OPT_SUPPRESS_INTERACTION] = DEF_SUPPRESS_INTERACTION;
	mConfigOpts[OPT_MINSTEP] = UNDEFINED;
	mConfigOpts[OPT_MAXSTEP] = UNDEFINED;
	mConfigOpts[OPT_QUERY] = UNDEFINED;
	mConfigOpts[OPT_MAXADDITIVE] = UNDEFINED;
	mConfigOpts[OPT_NUM_SOLN] = DEF_NUM_SOLN;
	mConfigOpts[OPT_EXT_PORT] = UNDEFINED;
	mConfigOpts[OPT_INT_PORT] = DEF_INTERNAL_PORT;
	mConfigOpts[OPT_SHIFT] = DEF_SHIFT;
	mConfigOpts[OPT_STATS] = DEF_STATS;

	memset(mCustomConfig,0,OPT_LENGTH);

	mCommands[TC_TRANSLATOR] = DEF_TRANS_CMD;
	mCommands[TC_PREPROC] = DEF_PREPROC_CMD;
	mCommands[TC_GROUNDER] = DEF_GRD_CMD;
	mCommands[TC_SOLVER] = DEF_SO_CMD;
	mCommands[TC_POSTPROC] = DEF_POSTPROC_CMD;
	mCommands[TC_REACTIVE_BRIDGE] = DEF_REACTIVE_BRIDGE_CMD;


	memset(mCustomCmd,0,TC_LENGTH);

	memset(mRun,1,TC_LENGTH);
	mRun[TC_GROUNDER] = false;

	memset(mCustomRun,0,TC_LENGTH);

	mOpts[TC_TRANSLATOR] = DEF_TRANS_OPTS;
	mOpts[TC_PREPROC] = DEF_PREPROC_OPTS;
	mOpts[TC_GROUNDER] = DEF_GRD_OPTS;
	mOpts[TC_SOLVER] = DEF_SO_OPTS;
	mOpts[TC_POSTPROC] = DEF_POSTPROC_OPTS;
	mOpts[TC_REACTIVE_BRIDGE] = DEF_REACTIVE_BRIDGE_OPTS;

	memset(mCustomOpts,0,TC_LENGTH);

	for (int i = (int)TC_BEGIN; i < (int)TC_LENGTH; i++) {
		mOutFiles[(Toolchain)i] = (temppath / fs::unique_path()).native();
	}

	memset(mCustomOut, 0, TC_LENGTH);

	// Default queries which should always be made available...
	mQueries[Config::TRANSITIONS_QUERY] = Query(Config::TRANSITIONS_QUERY, "System transitions", CONST_QUERY_TRANSITIONS, 1, 1);
	mQueries[Config::STATES_QUERY] = Query(Config::STATES_QUERY, "System states", CONST_QUERY_STATES, 0, 0);
	mQueries[Config::NO_QUERY] = Query(Config::NO_QUERY, "Satisfiability Check", CONST_QUERY_NONE, UNDEFINED, UNDEFINED);
}

// Install directory
std::string const& Config::installDir() const {
	return mPath;
/*
	char tmp[PATH_MAX];
	readlink("/proc/self/exe", tmp, PATH_MAX);
	std::string ret(tmp);
	ret = ret.substr(0,ret.find_last_of("/"));
	return ret;
*/
}

// standard file
std::string Config::stdFile() const { 
	fs::path path(installDir());

	if (mode() == MODE_INCREMENTAL || mode() == MODE_REACTIVE)
		return (path / CCALC2_ASP_STD_DYNAMIC_FILE).native();
	else
		return (path / CCALC2_ASP_STD_FILE).native();
}

// Additive file
inline std::string Config::additiveFile() const {
	fs::path path(installDir());

	if (mode() == MODE_INCREMENTAL || mode() == MODE_REACTIVE)
		return (path / CCALC2_ASP_ADDITIVE_DYNAMIC_FILE).native();
	else
		return (path / CCALC2_ASP_ADDITIVE_FILE).native();
}


// Gets the definition of a constant or NULL
std::string const* Config::def(std::string const& constant) const {
	try {
		return &mConstDefs.at(constant);
	}
	catch (std::out_of_range& e)
	{
		return NULL;
	}
	return NULL;
}


// Gets the query specified by the ID, or null if it doesn't exist.
Config::Query const* Config::query(unsigned int queryID) const {
	if (hasQuery(queryID)) return &(mQueries.at(queryID));
	else return NULL;
}

// Convenience method to get the name of a query.
std::string const* Config::queryName(unsigned int queryID) const {
	Query const* q;
	if ((q = query(queryID)) != NULL) return &(q->name);
	else return NULL;
}

// Convenience method to get the maxstep of a query
unsigned int Config::queryMaxStep(unsigned int queryID) const {
	Query const* q;
	if ((q = query(queryID)) != NULL) return q->maxstep;
	else return UNDEFINED;
}

// Convenience method to get the minstep of a query.
unsigned int Config::queryMinStep(unsigned int queryID) const {
	Query const* q;
	if ((q = query(queryID)) != NULL) return q->minstep;
	else return UNDEFINED;
}



// Set the running mode for the program
bool Config::mode(Mode newMode) {
	mCustomRunningMode++;
	if (mRunningMode == newMode) return false;
	mRunningMode = newMode;


	switch (newMode) {
	case MODE_STATIC_AUTO:
	case MODE_STATIC_MANUAL:
		if (!customCmd(TC_GROUNDER)) mCommands[TC_GROUNDER] = DEF_GRD_STATIC_CMD;
		if (!customCmd(TC_SOLVER)) mCommands[TC_SOLVER] = DEF_SO_STATIC_CMD;
		break;
	case MODE_INCREMENTAL:
		if (!customCmd(TC_GROUNDER)) mCommands[TC_GROUNDER] = DEF_GRD_INCREMENTAL_CMD;
		if (!customCmd(TC_SOLVER)) mCommands[TC_SOLVER] = DEF_SO_INCREMENTAL_CMD;
		break;
	case MODE_REACTIVE:
		if (!customCmd(TC_GROUNDER)) mCommands[TC_GROUNDER] = DEF_GRD_REACTIVE_CMD;
		if (!customCmd(TC_POSTPROC)) mRun[TC_POSTPROC] = false;
		if (!customCmd(TC_SOLVER)) mCommands[TC_SOLVER] = DEF_SO_REACTIVE_CMD;
		break;
	case MODE_UNKNOWN:
		break;
	}

	return (bool)(mCustomRunningMode - 1);
 }



// Parses the language represented in the string.
bool Config::parseLang(char const* str, Language& outLang) {
	if (boost::iequals(str, "bc")) outLang = LANG_BC;
	else if (boost::iequals(str, "c+")) outLang = LANG_CPLUS;
	else return false;
	return true;
}

// Set the input language for the program
bool Config::lang(Language newLang) {
	mCustomLang++;
	if (mLang == newLang) return false;
	mLang = newLang;
	return (bool)(mCustomLang - 1);
 }

// Checks if a constant is defined
inline bool Config::isDefined(std::string const& constant) const {
	std::string constant_trimmed = trimWhitespace(constant);

	if (constant_trimmed == CONST_MAXSTEP) {
		return customConfigOpt(OPT_MAXSTEP);
	} else if (constant_trimmed == CONST_MINSTEP) {
		return customConfigOpt(OPT_MINSTEP);
	} else if (constant_trimmed == CONST_QUERY) {
		return customConfigOpt(OPT_QUERY);
	} else if (constant_trimmed == CONST_MAXADDITIVE) {
		return customConfigOpt(OPT_MAXADDITIVE);
	} else {
		return (bool)mConstDefs.count(constant_trimmed);
	}
}

// Sets a config options to a boolean value.
bool Config::boolConfigOpt(ConfigOption opt, bool val) {
	mCustomConfig[opt]++;
	if (((bool)mConfigOpts[opt]) == val) return false;
	mConfigOpts[opt] = (unsigned int)val;
	return (bool)(mCustomConfig[opt] - 1);
}

// Sets a config option to an integral value.
bool Config::intConfigOpt(ConfigOption opt, unsigned int val) {
	mCustomConfig[opt]++;
	if (mConfigOpts[opt] == val) return false;
	mConfigOpts[opt] = val;
	return (bool)(mCustomConfig[opt] - 1);
}

// Sets a command option to the provided value.
bool Config::command(Toolchain tool, std::string const& val) {
	bool ret = mCustomCmd[tool]++;
	if (mCommands[tool] == val) return run(tool, val != "");
	ret |= run(tool, val != "");
	mCommands[tool] = val;
	return ret;
}

/// @return If the run status has been previously changed.
bool Config::run(Toolchain tool, bool val) {
	mCustomRun[tool]++;
	if(mRun[tool] == val) return false;
	mRun[tool] = val;
	return (bool)(mCustomRun[tool] - 1);
}

bool Config::opts(Toolchain tool, std::string const& val) {
	mCustomOpts[tool]++;
	if (mOpts[tool] == val) return false;
	mOpts[tool] = val;
	return (bool)(mCustomOpts[tool] - 1);
}


// Sets the target output file for the toolchain component.
bool Config::output(Toolchain tool, std::string const& file) {
	mCustomOut[tool]++;
	if (mOutFiles[tool] == file) return false;
	mOutFiles[tool] = file;
	return (bool)(mCustomOut[tool] - 1);
}
	
// Sets a new value to use as a none alias.
bool Config::noneAlias(std::string const& alias) {
	mCustomNone++;
	if (mNoneAlias == alias) return false;
	mNoneAlias = alias;
	return (bool)(mCustomNone - 1);
}

// Sets the file to output the symbol table to.
bool Config::symTabFile(std::string const& file) {
	mCustomSymTab++;
	if (mSymTabFile == file) return false;
	mSymTabFile = file;
	return (bool)(mCustomSymTab - 1);
}

// Defines a constant
bool Config::def(std::string const& constant, std::string const& value) {
	std::string constant_trimmed = trimWhitespace(constant);
	std::string value_trimmed = trimWhitespace(value);
	unsigned int tmpVal;
	bool redef = false;
	bool bad = false;

	// Check this against several known constants and update their values if necessary...
	if (constant_trimmed == CONST_MAXSTEP) {
		std::pair<unsigned int,unsigned int> stepvals = parseMaxstep(value_trimmed);

		if (stepvals.second == UNDEFINED) {
			bad = true;
		} else {
			redef = intConfigOpt(OPT_MAXSTEP, stepvals.second);
			if (stepvals.first != UNDEFINED) intConfigOpt(OPT_MINSTEP, stepvals.first);
		}

	} else if (constant_trimmed == CONST_MINSTEP) {
		if (from_string(tmpVal,value_trimmed)) {
			redef = intConfigOpt(OPT_MINSTEP, tmpVal);
		} else {
			// Oops... Throw a warning, ignore it, and continue.
			bad = true;
		}
	} else if (constant_trimmed == CONST_QUERY) {

		// The query allows for some special options...
		if (value_trimmed == CONST_QUERY_NONE) {
			// The user has requested we use no query.
			redef = intConfigOpt(OPT_QUERY, NO_QUERY);
		} else if (value_trimmed == CONST_QUERY_STATES) {
			// The user has requested that we find the sytem's states.
			redef = intConfigOpt(OPT_QUERY, STATES_QUERY);
		} else if (value_trimmed == CONST_QUERY_TRANSITIONS) {
			// The user has requested that we find the system's transitions.
			redef = intConfigOpt(OPT_QUERY, TRANSITIONS_QUERY);
		} else if (from_string(tmpVal,value_trimmed)) {
			// The user has provided us with a query to perform.
			redef = intConfigOpt(OPT_QUERY,tmpVal);
		} else {
			// Oops... Throw a warning, ignore it, and continue.
			bad = true;
		}
	} else if (constant_trimmed == CONST_MAXADDITIVE) {
		if (from_string(tmpVal,value_trimmed)) {
			redef = intConfigOpt(OPT_MAXADDITIVE,tmpVal);
		} else {
			// Oops... Throw a warning, ignore it, and continue.
			bad = true;
		}
	} else {
		// It's a normal constant...
		redef = (bool)mConstDefs.count(constant_trimmed);
		mConstDefs[constant_trimmed] = value_trimmed;

	}

	if (bad) {
		// If the value was bad then go ahead and warn the user.
		warning("The value '%s' is not valid for the reserved constant '%s'. It will be ignored.",true, value_trimmed.c_str() ,constant_trimmed.c_str());
	}

	return redef;
}

// Calculates the number of input files for a specific tool
size_t Config::cumulativeInputCount(Toolchain tool) const {

	size_t count = 0;

	// Let's count all the input files
	// which belong to
	// a) The current tool,
	// b) The tools immediately preceeding the tool which we aren't running.

	for (Toolchain i = tool; i >= TC_BEGIN; i = (Toolchain)((int)i - 1)) {
		// Only include files which haven't been included by a previous tool.
		if (i != tool && run(i)) break;

		for (InputList::const_iterator it = beginInput((Toolchain)i); it != endInput((Toolchain)i); it++) {
			count++;
		}
	}

	// Compile the input files from previous step's outputs.
	switch (tool) {
	case TC_END:
	case TC_REACTIVE_BRIDGE:
		// Reactive bridge is different.
		if (run(TC_TRANSLATOR)) {
			count++;
		}
		break;

	case TC_POSTPROC:
		if (run(TC_SOLVER)) {
			count++;
			break;
		}
		/* no break */

	case TC_SOLVER:
		if (run(TC_GROUNDER)) {
			count++;
			break;
		}
		/* no break */

	case TC_GROUNDER:
		if (run(TC_PREPROC)) {
			count++;
			break;
		}
		/* no break */

	case TC_PREPROC:
		if (run(TC_TRANSLATOR)) {
			count++;
			break;
		}
		/* no break */

	case TC_TRANSLATOR:
		// none
		break;

	}

	return count;
}

// Compiles a command line call for the given toolchain element
std::string Config::compileCommandLine(Toolchain tool, RunConfig const* subconfig) const {

	std::ostringstream out;
	std::list<std::string> tmp;
	compileArgs(tool, tmp, subconfig);

	out << command(tool);

	for (std::string const& arg : tmp) {
		out << " " << arg;
	}

	return out.str();
}

// Compiles the set of arguments used to call a specific element in the toolchain.
void Config::compileArgs(Toolchain tool, std::list<std::string>& arglist, RunConfig const* subconfig)  const {


	std::stringstream tmpstream;
	std::string tmpstr;

	tmpstream << opts(tool);

	while (tmpstream) {
		tmpstream >> tmpstr;
		arglist.push_back(tmpstr);
	}

	// Add step specific instructions
	switch (tool) {
	case TC_END:
	case TC_TRANSLATOR:
		compileInputArgs(tool, arglist);

		// The translator should be told to output the symbol table
		//TODO
		arglist.push_back("--symbol-table");
		arglist.push_back(symTabFile());

		// We also should have it tell us what special constants it finds.
		arglist.push_back("--expressive-return");

		switch (lang()) {
		case LANG_CPLUS:
			arglist.push_back("--language=c+");
			break;

		case LANG_BC:
			arglist.push_back("--language=bc");
			break;
		}


		// The translator should be told the translation mode.
		switch (mode()) {
		case MODE_STATIC_AUTO:
		case MODE_STATIC_MANUAL:
			arglist.push_back("--static");
			break;
		case MODE_INCREMENTAL:
		case MODE_REACTIVE:
		default:
			break;
		}

		break;

	case TC_PREPROC:

		arglist.push_back("-i");

		// The preprocessor should include the standard files in addition to the 'normal' include files.
		if (boolConfigOpt(OPT_INCL_STD))
			arglist.push_back(stdFile());
		if (boolConfigOpt(OPT_INCL_ADDITIVE))
			arglist.push_back(additiveFile());
		
		compileInputArgs(tool, arglist);

		break;

	case TC_SOLVER:
		// Solvers need:
		// * Number of solutions to find.
		// * Query information.
		// * Minimum step information
		// * Maximum step information.
		compileInputArgs(tool, arglist);

		// # solutions
		if (subconfig) {
			arglist.push_back(to_string(subconfig->numSoln));
		} else {
			arglist.push_back(to_string(intConfigOpt(OPT_NUM_SOLN)));
		}

		// special constants (if we're running what looks like a grounder/solver)
		if (!run(TC_GROUNDER)) {
			if (subconfig) {
				if (subconfig->queryId != UNDEFINED) arglist.push_back("-c query=" + to_string(subconfig->queryId));
				if (subconfig->maxstep != UNDEFINED) arglist.push_back("-c maxstep=" + to_string(subconfig->maxstep));
				if (subconfig->minstep != UNDEFINED) arglist.push_back("-c minstep=" + to_string(subconfig->minstep));
			} else {
				if (intConfigOpt(OPT_QUERY) != UNDEFINED) arglist.push_back("-c query=" + to_string(intConfigOpt(OPT_QUERY)));
				if (intConfigOpt(OPT_MAXSTEP) != UNDEFINED) arglist.push_back("-c maxstep=" + to_string(intConfigOpt(OPT_MAXSTEP)));
				if (intConfigOpt(OPT_MINSTEP) != UNDEFINED) arglist.push_back("-c minstep=" + to_string(intConfigOpt(OPT_MINSTEP)));
			}


			if (intConfigOpt(OPT_MAXADDITIVE) != UNDEFINED) arglist.push_back("-c maxAdditive=" + to_string(intConfigOpt(OPT_MAXADDITIVE)));
			if (noneAlias() != "") arglist.push_back("-c none=" + noneAlias());

			// The '--shift' flag signals that gringo should shift disjunction into the body of a rule.
			if (boolConfigOpt(OPT_SHIFT)) arglist.push_back("--shift");


			for (ConstantMap::const_iterator it = beginConstants(); it != endConstants(); it++) {
				arglist.push_back("-c");
				arglist.push_back(it->first + "=" + it->second);
			}
		}

		// min/max step
		if (mode() == MODE_INCREMENTAL || mode() == MODE_REACTIVE) {
			if (subconfig) {
				if (subconfig->minstep != UNDEFINED) arglist.push_back("--imin=" + to_string(subconfig->minstep));
				if (subconfig->maxstep != UNDEFINED) {
					if (subconfig->maxstep > 0)
						arglist.push_back("--imax=" + to_string(subconfig->maxstep));
					else
						arglist.push_back("--ibase");
				}
			} else {
				if (intConfigOpt(OPT_MINSTEP) != UNDEFINED) arglist.push_back("--imin=" + to_string(intConfigOpt(OPT_MINSTEP)));
				if (intConfigOpt(OPT_MAXSTEP) != UNDEFINED){
					if (intConfigOpt(OPT_MAXSTEP) > 0)
						arglist.push_back("--imax=" + to_string(intConfigOpt(OPT_MAXSTEP)));
					else
						arglist.push_back("--ibase");
				}
			}
		}

		// The --stats option
		if (boolConfigOpt(OPT_STATS)) {
			arglist.push_back("--stats");

			if (mode() == MODE_REACTIVE || mode() == MODE_INCREMENTAL)
				arglist.push_back("--istats");
		}


		if (mode() == MODE_REACTIVE && run(TC_REACTIVE_BRIDGE)) {

#ifdef NO_REACTIVE_BRIDGE
			if (intConfigOpt(OPT_EXT_PORT) != UNDEFINED)
				arglist.push_back("--port=" + to_string(intConfigOpt(OPT_EXT_PORT)));
			else
#endif
				arglist.push_back("--port=" + to_string(intConfigOpt(OPT_INT_PORT)));
		} else if (mode() == MODE_REACTIVE) {
			// we aren't running the reactive bridge.
			// connect to the external port instead.
			arglist.push_back("--port=" + to_string(intConfigOpt(OPT_EXT_PORT)));
		} 
			

		break;
	case TC_GROUNDER:
		compileInputArgs(tool, arglist);

		if (subconfig) {
			if (subconfig->queryId != UNDEFINED) arglist.push_back("-c query=" + to_string(subconfig->queryId));
			if (subconfig->maxstep != UNDEFINED) arglist.push_back("-c maxstep=" + to_string(subconfig->maxstep));
			if (subconfig->minstep != UNDEFINED) arglist.push_back("-c minstep=" + to_string(subconfig->minstep));
		} else {
			if (intConfigOpt(OPT_QUERY) != UNDEFINED) arglist.push_back("-c query=" + to_string(intConfigOpt(OPT_QUERY)));
			if (intConfigOpt(OPT_MAXSTEP) != UNDEFINED) arglist.push_back("-c maxstep=" + to_string(intConfigOpt(OPT_MAXSTEP)));
			if (intConfigOpt(OPT_MINSTEP) != UNDEFINED) arglist.push_back("-c minstep=" + to_string(intConfigOpt(OPT_MINSTEP)));
		}


		if (intConfigOpt(OPT_MAXADDITIVE) != UNDEFINED) arglist.push_back("-c maxAdditive=" + to_string(intConfigOpt(OPT_MAXADDITIVE)));
		if (noneAlias() != "") arglist.push_back("-c none=" + noneAlias());

		// The '--shift' flag signals that gringo should shift disjunction into the body of a rule.
		if (boolConfigOpt(OPT_SHIFT)) arglist.push_back("--shift");


		for (ConstantMap::const_iterator it = beginConstants(); it != endConstants(); it++) {
			arglist.push_back("-c");
			arglist.push_back(it->first + "=" + it->second);
		}
		break;

	case TC_REACTIVE_BRIDGE:
#ifdef NO_REACTIVE_BRIDGE
		// Don't do anything!
#else
		// The reactive client doesn't need to know much.
		// Just the ports to use and where to find the symbol table
		compileInputArgs(tool, arglist);

		arglist.push_back("--port=" + to_string(intConfigOpt(OPT_INT_PORT)));

		if (intConfigOpt(OPT_EXT_PORT) != UNDEFINED)
			arglist.push_back("--client-port=" + to_string(intConfigOpt(OPT_EXT_PORT)));
		if (noneAlias() != "") 
			arglist.push_back("--none=" + noneAlias());
#endif
		break;

	case TC_POSTPROC:
		compileInputArgs(tool, arglist);
		if (noneAlias() != "")
			arglist.push_back("--none=" + noneAlias());
		break;
	}
}

// Sets the configuration to run up to (and including) the provided tool.
bool Config::setRunTo(Toolchain tool) {
	bool conflict = false;

//	for (int i = (int)TC_BEGIN; i < (int)tool+1; i++) {
//		conflict |= run((Toolchain)i, true);
//	}
	for (int i = ((int)tool)+1; i < (int)TC_END; i++) {
		conflict |= run((Toolchain)i, false);
	}
	return conflict;
}

// Sets the configuration to run from the provided tool.
bool Config::setRunFrom(Toolchain tool) {
	bool conflict = false;

	for (int i = (int)TC_BEGIN; i < (int)tool; i++) {
		conflict |= run((Toolchain)i, false);
	}
//	for (int i = (int)tool; i < (int)TC_END; i++) {
//		conflict |= run((Toolchain)i, true);
//	}
	return conflict;
}

// Sets the configuration to run only the provided tool.
bool Config::setOnlyRun(Toolchain tool) {
	bool conflict = false;

	for (int i = (int)TC_BEGIN; i < (int)tool; i++) {
		conflict |= run((Toolchain)i, false);
	}
	conflict |= run(tool,true);
	for (int i = ((int)tool)+1; i < (int)TC_END; i++) {
		conflict |= run((Toolchain)i, false);
	}
	return conflict;
}

// Attempts to add a query to the list of known queries.
bool Config::addQuery(unsigned int queryID, std::string const& name, std::string const& cmd, unsigned int maxstep, unsigned int minstep) {
	bool unique = true;

	// ensure that the query is not already present...
	if (hasQuery(queryID)) unique = false;

	if (unique) {
		mQueries[queryID] = Query(queryID,name,cmd,minstep,maxstep);
	}

	return unique;
}

// Attempts to add a query to the list of known queries.
bool Config::addQuery(Query const& query) {
	bool unique = true;

	// ensure that the query is not already present...
	if (hasQuery(query.id)) unique = false;

	if (unique) {
		mQueries[query.id] = query;
	}

	return unique;
}



Config::FileType Config::addInput(std::string const& input) {

	char const* input_cstr = input.c_str();
	FileType type(false,TC_BEGIN, MODE_UNKNOWN);

	debug("Adding input file '%s'.", true, input.c_str());

	// Try to find the extension the file extension.
	size_t pos = input.find_last_of('.');
	if (pos == std::string::npos) {
		// No file extension to speak of, default to the beginning of the toolchain.
		mInputs[TC_BEGIN].push_back(input);
		return type;
	}

	// We found an extension, let's test it against known extensions
	checkExtension(&input_cstr[pos], type);

	if (type.known) {
		debug("Found the extension. It belongs to toolchain component '%d'", true, type.tool);
		mInputs[type.tool].push_back(input);
	} else {
		debug("Unknown extension.", true);
		// Default to the beginning of the toolchain if we don't know what it is.
		mInputs[TC_BEGIN].push_back(input);
	}


	return type;
}

/// Parses a maxstep specification.
std::pair<unsigned int, unsigned int> Config::parseMaxstep(std::string const& maxstep) {
	size_t pos;
	std::pair<unsigned int, unsigned int> ret(UNDEFINED, UNDEFINED);

	std::string tmp;

	if ((pos = maxstep.find("..")) != std::string::npos) {
		// They've provided a range. Interpret it as a minstep and maxstep.
		tmp = trimWhitespace(maxstep.substr(pos+2));

	} else {
		tmp = maxstep;
	}

	if (tmp == CONST_MAXSTEP_INFINITE) {
		ret.second = INF;	
	} else  if (!from_string(ret.second,tmp)) {
		// error
		ret.first = UNDEFINED;
		ret.second = UNDEFINED;
	}

	if (ret.second != UNDEFINED 
		&& pos != std::string::npos 
		&& !from_string(ret.first,trimWhitespace(maxstep.substr(0,pos)))) {
		// error
		ret.first = UNDEFINED;
		ret.second = UNDEFINED;

	}

	return ret;
}

void Config::compileInputArgs(Toolchain tool, std::list<std::string>& args) const {
	// Otherwise, we need to aggregate all the input files
	// which belong to
	// a) The current tool,
	// b) The tools immediately preceeding the tool which we aren't running.

	// Compile the input files from previous step's outputs.
	switch (tool) {
	case TC_END:
	case TC_REACTIVE_BRIDGE:
		// Reactive bridge is different.
		// TODO Finalize this
		if (run(TC_TRANSLATOR)) {
//			out << " " << intTransSymTabFile();
		}
		break;

	case TC_POSTPROC:
		if (run(TC_SOLVER)) {
			args.push_back(output(TC_SOLVER));
			break;
		}
		/* no break */

	case TC_SOLVER:
		if (run(TC_GROUNDER)) {
			args.push_back(output(TC_GROUNDER));
			break;
		}
		/* no break */

	case TC_GROUNDER:
		if (run(TC_PREPROC)) {
			args.push_back(output(TC_PREPROC));
			break;
		}
		/* no break */

	case TC_PREPROC:
		if (run(TC_TRANSLATOR)) {
			args.push_back(output(TC_TRANSLATOR));
			break;
		}
		/* no break */

	case TC_TRANSLATOR:
		// none
		break;

	}

	for (int i = (int)tool; i >= (int)TC_BEGIN; i--) {
		// Only include files which haven't been included by a previous tool.
		if (i != (int)tool && run((Toolchain)i)) break;

		for (InputList::const_iterator it = beginInput((Toolchain)i); it != endInput((Toolchain)i); it++) {
			args.push_back(*it);
		}
	}
}


Config::FileType& Config::checkExtension(char const* ext, FileType& out) const {
	char const* marker;

	out.known = true;
	out.runningMode = MODE_UNKNOWN;
	out.tool = TC_BEGIN;



	// Skip the period if they included it.
	if (ext[0] == '.') ext++;

	debug("Checking extension '%s'.", true, ext);

	
#line 919 "<stdout>"
	{
		char yych;
		unsigned int yyaccept = 0;

		yych = *ext;
		switch (yych) {
		case '\n':	goto yy2;
		case 'a':	goto yy5;
		case 'c':	goto yy3;
		case 'f':	goto yy8;
		case 'g':	goto yy10;
		case 'i':	goto yy7;
		case 'l':	goto yy9;
		case 'o':	goto yy6;
		case 's':	goto yy11;
		default:	goto yy12;
		}
yy2:
		ext = marker;
		switch (yyaccept) {
		case 0: 	goto yy4;
		case 1: 	goto yy37;
		case 2: 	goto yy62;
		case 3: 	goto yy88;
		}
yy3:
		++ext;
		switch ((yych = *ext)) {
		case '+':	goto yy85;
		case 'p':	goto yy87;
		default:	goto yy4;
		}
yy4:
#line 953 "Config.re2c"
		{ out.known = false; return out;}
#line 955 "<stdout>"
yy5:
		yyaccept = 0;
		yych = *(marker = ++ext);
		switch (yych) {
		case 'a':	goto yy57;
		case 'c':	goto yy60;
		case 'f':	goto yy59;
		case 'g':	goto yy56;
		case 'l':	goto yy58;
		case 's':	goto yy81;
		default:	goto yy4;
		}
yy6:
		yyaccept = 0;
		yych = *(marker = ++ext);
		switch (yych) {
		case 'a':	goto yy57;
		case 'c':	goto yy60;
		case 'f':	goto yy59;
		case 'g':	goto yy56;
		case 'l':	goto yy58;
		default:	goto yy4;
		}
yy7:
		yyaccept = 0;
		yych = *(marker = ++ext);
		switch (yych) {
		case 'a':	goto yy32;
		case 'c':	goto yy35;
		case 'f':	goto yy34;
		case 'g':	goto yy31;
		case 'l':	goto yy33;
		default:	goto yy4;
		}
yy8:
		yyaccept = 0;
		yych = *(marker = ++ext);
		switch (yych) {
		case '2':	goto yy27;
		default:	goto yy4;
		}
yy9:
		yyaccept = 0;
		yych = *(marker = ++ext);
		switch (yych) {
		case 'p':	goto yy22;
		case 'u':	goto yy24;
		default:	goto yy4;
		}
yy10:
		yyaccept = 0;
		yych = *(marker = ++ext);
		switch (yych) {
		case 'r':	goto yy19;
		default:	goto yy4;
		}
yy11:
		yyaccept = 0;
		yych = *(marker = ++ext);
		switch (yych) {
		case 'y':	goto yy13;
		default:	goto yy4;
		}
yy12:
		yych = *++ext;
		goto yy4;
yy13:
		yych = *++ext;
		switch (yych) {
		case 'm':	goto yy14;
		default:	goto yy2;
		}
yy14:
		yych = *++ext;
		switch (yych) {
		case 't':	goto yy15;
		default:	goto yy2;
		}
yy15:
		yych = *++ext;
		switch (yych) {
		case 'a':	goto yy16;
		default:	goto yy2;
		}
yy16:
		yych = *++ext;
		switch (yych) {
		case 'b':	goto yy17;
		default:	goto yy2;
		}
yy17:
		++ext;
#line 951 "Config.re2c"
		{ out.tool = TC_REACTIVE_BRIDGE; out.runningMode = MODE_REACTIVE; return out;}
#line 1050 "<stdout>"
yy19:
		yych = *++ext;
		switch (yych) {
		case 'd':	goto yy20;
		default:	goto yy2;
		}
yy20:
		++ext;
#line 945 "Config.re2c"
		{ out.tool = TC_SOLVER; out.runningMode = MODE_STATIC_AUTO | MODE_STATIC_MANUAL; return out;}
#line 1061 "<stdout>"
yy22:
		++ext;
#line 937 "Config.re2c"
		{ out.tool = TC_GROUNDER; out.runningMode = MODE_STATIC_AUTO | MODE_STATIC_MANUAL; return out;}
#line 1066 "<stdout>"
yy24:
		yych = *++ext;
		switch (yych) {
		case 'a':	goto yy25;
		default:	goto yy2;
		}
yy25:
		++ext;
#line 936 "Config.re2c"
		{ out.tool = TC_GROUNDER; out.runningMode = MODE_STATIC_AUTO | MODE_STATIC_MANUAL | MODE_INCREMENTAL | MODE_REACTIVE; return out; }
#line 1077 "<stdout>"
yy27:
		yych = *++ext;
		switch (yych) {
		case 'l':	goto yy28;
		default:	goto yy2;
		}
yy28:
		yych = *++ext;
		switch (yych) {
		case 'p':	goto yy29;
		default:	goto yy2;
		}
yy29:
		++ext;
#line 932 "Config.re2c"
		{ out.tool = TC_PREPROC; out.runningMode = MODE_STATIC_AUTO | MODE_STATIC_MANUAL; return out; }
#line 1094 "<stdout>"
yy31:
		yych = *++ext;
		switch (yych) {
		case 'r':	goto yy53;
		default:	goto yy2;
		}
yy32:
		yych = *++ext;
		switch (yych) {
		case 's':	goto yy50;
		default:	goto yy2;
		}
yy33:
		yych = *++ext;
		switch (yych) {
		case 'p':	goto yy48;
		default:	goto yy2;
		}
yy34:
		yych = *++ext;
		switch (yych) {
		case '2':	goto yy44;
		default:	goto yy2;
		}
yy35:
		yych = *++ext;
		switch (yych) {
		case '+':	goto yy38;
		case 'p':	goto yy36;
		default:	goto yy2;
		}
yy36:
		yyaccept = 1;
		yych = *(marker = ++ext);
		switch (yych) {
		case 'l':	goto yy40;
		default:	goto yy37;
		}
yy37:
#line 928 "Config.re2c"
		{ out.tool = TC_TRANSLATOR; out.runningMode = MODE_INCREMENTAL; return out;}
#line 1136 "<stdout>"
yy38:
		++ext;
#line 930 "Config.re2c"
		{ out.tool = TC_TRANSLATOR; out.runningMode = MODE_INCREMENTAL; return out;}
#line 1141 "<stdout>"
yy40:
		yych = *++ext;
		switch (yych) {
		case 'u':	goto yy41;
		default:	goto yy2;
		}
yy41:
		yych = *++ext;
		switch (yych) {
		case 's':	goto yy42;
		default:	goto yy2;
		}
yy42:
		++ext;
#line 929 "Config.re2c"
		{ out.tool = TC_TRANSLATOR; out.runningMode = MODE_INCREMENTAL; return out;}
#line 1158 "<stdout>"
yy44:
		yych = *++ext;
		switch (yych) {
		case 'l':	goto yy45;
		default:	goto yy2;
		}
yy45:
		yych = *++ext;
		switch (yych) {
		case 'p':	goto yy46;
		default:	goto yy2;
		}
yy46:
		++ext;
#line 934 "Config.re2c"
		{ out.tool = TC_PREPROC; out.runningMode = MODE_INCREMENTAL; return out;}
#line 1175 "<stdout>"
yy48:
		++ext;
#line 939 "Config.re2c"
		{ out.tool = TC_GROUNDER; out.runningMode = MODE_INCREMENTAL; return out;}
#line 1180 "<stdout>"
yy50:
		yych = *++ext;
		switch (yych) {
		case 'p':	goto yy51;
		default:	goto yy2;
		}
yy51:
		++ext;
#line 943 "Config.re2c"
		{ out.tool = TC_GROUNDER; out.runningMode = MODE_INCREMENTAL; return out;}
#line 1191 "<stdout>"
yy53:
		yych = *++ext;
		switch (yych) {
		case 'd':	goto yy54;
		default:	goto yy2;
		}
yy54:
		++ext;
#line 947 "Config.re2c"
		{ out.tool = TC_SOLVER; out.runningMode = MODE_INCREMENTAL; return out;}
#line 1202 "<stdout>"
yy56:
		yych = *++ext;
		switch (yych) {
		case 'r':	goto yy78;
		default:	goto yy2;
		}
yy57:
		yych = *++ext;
		switch (yych) {
		case 's':	goto yy75;
		default:	goto yy2;
		}
yy58:
		yych = *++ext;
		switch (yych) {
		case 'p':	goto yy73;
		default:	goto yy2;
		}
yy59:
		yych = *++ext;
		switch (yych) {
		case '2':	goto yy69;
		default:	goto yy2;
		}
yy60:
		yych = *++ext;
		switch (yych) {
		case '+':	goto yy63;
		case 'p':	goto yy61;
		default:	goto yy2;
		}
yy61:
		yyaccept = 2;
		yych = *(marker = ++ext);
		switch (yych) {
		case 'l':	goto yy65;
		default:	goto yy62;
		}
yy62:
#line 925 "Config.re2c"
		{ out.tool = TC_TRANSLATOR; out.runningMode = MODE_REACTIVE; return out;}
#line 1244 "<stdout>"
yy63:
		++ext;
#line 927 "Config.re2c"
		{ out.tool = TC_TRANSLATOR; out.runningMode = MODE_REACTIVE; return out;}
#line 1249 "<stdout>"
yy65:
		yych = *++ext;
		switch (yych) {
		case 'u':	goto yy66;
		default:	goto yy2;
		}
yy66:
		yych = *++ext;
		switch (yych) {
		case 's':	goto yy67;
		default:	goto yy2;
		}
yy67:
		++ext;
#line 926 "Config.re2c"
		{ out.tool = TC_TRANSLATOR; out.runningMode = MODE_REACTIVE; return out;}
#line 1266 "<stdout>"
yy69:
		yych = *++ext;
		switch (yych) {
		case 'l':	goto yy70;
		default:	goto yy2;
		}
yy70:
		yych = *++ext;
		switch (yych) {
		case 'p':	goto yy71;
		default:	goto yy2;
		}
yy71:
		++ext;
#line 933 "Config.re2c"
		{ out.tool = TC_PREPROC; out.runningMode = MODE_REACTIVE; return out;}
#line 1283 "<stdout>"
yy73:
		++ext;
#line 938 "Config.re2c"
		{ out.tool = TC_GROUNDER; out.runningMode = MODE_REACTIVE; return out;}
#line 1288 "<stdout>"
yy75:
		yych = *++ext;
		switch (yych) {
		case 'p':	goto yy76;
		default:	goto yy2;
		}
yy76:
		++ext;
#line 942 "Config.re2c"
		{ out.tool = TC_GROUNDER; out.runningMode = MODE_REACTIVE; return out;}
#line 1299 "<stdout>"
yy78:
		yych = *++ext;
		switch (yych) {
		case 'd':	goto yy79;
		default:	goto yy2;
		}
yy79:
		++ext;
#line 946 "Config.re2c"
		{ out.tool = TC_SOLVER; out.runningMode = MODE_REACTIVE; return out;}
#line 1310 "<stdout>"
yy81:
		++ext;
		switch ((yych = *ext)) {
		case 'p':	goto yy83;
		default:	goto yy82;
		}
yy82:
#line 949 "Config.re2c"
		{ out.tool = TC_POSTPROC; out.runningMode = MODE_STATIC_AUTO | MODE_STATIC_MANUAL | MODE_INCREMENTAL; return out;}
#line 1320 "<stdout>"
yy83:
		++ext;
#line 941 "Config.re2c"
		{ out.tool = TC_GROUNDER; out.runningMode = MODE_STATIC_AUTO | MODE_STATIC_MANUAL; return out;}
#line 1325 "<stdout>"
yy85:
		++ext;
#line 924 "Config.re2c"
		{ out.tool = TC_TRANSLATOR; out.runningMode = MODE_STATIC_AUTO | MODE_STATIC_MANUAL | MODE_INCREMENTAL; return out;}
#line 1330 "<stdout>"
yy87:
		yyaccept = 3;
		yych = *(marker = ++ext);
		switch (yych) {
		case 'l':	goto yy89;
		default:	goto yy88;
		}
yy88:
#line 922 "Config.re2c"
		{ out.tool = TC_TRANSLATOR; out.runningMode = MODE_STATIC_AUTO | MODE_STATIC_MANUAL | MODE_INCREMENTAL; return out;}
#line 1341 "<stdout>"
yy89:
		yych = *++ext;
		switch (yych) {
		case 'u':	goto yy90;
		default:	goto yy2;
		}
yy90:
		yych = *++ext;
		switch (yych) {
		case 's':	goto yy91;
		default:	goto yy2;
		}
yy91:
		++ext;
#line 923 "Config.re2c"
		{ out.tool = TC_TRANSLATOR; out.runningMode = MODE_STATIC_AUTO | MODE_STATIC_MANUAL | MODE_INCREMENTAL; return out;}
#line 1358 "<stdout>"
	}
#line 954 "Config.re2c"


	return out;
}
