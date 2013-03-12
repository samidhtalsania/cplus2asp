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


#include <string>

#include "Translator.h"
#include "DataStructs/datastruct_externs.h"
#include "DataStructs/Sort.h"
#include "DataStructs/Variable.h"


// Full constructor.
Variable::Variable(std::string const& name, Sort const* sort)
	: Element(name, Translator::sanitizeVariableName(name), Element::ELEM_VAR)
{
	mSortRef = sort;
}

// Standard toString function.
std::string Variable::toString() const
{
	std::string tempStr;
	tempStr += "[Variable]";
	tempStr += "\n  sortRef = ";
	// If we don't have a sortRef yet, just say NULL.
	if(mSortRef == NULL)
	{
		tempStr += "NULL";
	}
	else
	{
		tempStr += "\"";
		tempStr += mSortRef->fullName();
		tempStr += "\"";
	}
	return tempStr;
}
