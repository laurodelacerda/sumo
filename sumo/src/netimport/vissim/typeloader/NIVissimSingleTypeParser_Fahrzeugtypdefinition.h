/****************************************************************************/
/// @file    NIVissimSingleTypeParser_Fahrzeugtypdefinition.h
/// @author  Daniel Krajzewicz
/// @date    Wed, 18 Dec 2002
/// @version $Id$
///
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NIVissimSingleTypeParser_Fahrzeugtypdefinition_h
#define NIVissimSingleTypeParser_Fahrzeugtypdefinition_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include "../NIVissimLoader.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NIVissimSingleTypeParser_Fahrzeugtypdefinition
 *
 */
class NIVissimSingleTypeParser_Fahrzeugtypdefinition :
            public NIVissimLoader::VissimSingleTypeParser
{
public:
    /// Constructor
    NIVissimSingleTypeParser_Fahrzeugtypdefinition(NIVissimLoader &parent,
            NIVissimLoader::ColorMap &colorMap);

    /// Destructor
    ~NIVissimSingleTypeParser_Fahrzeugtypdefinition();

    /// Parses the data type from the given stream
    bool parse(std::istream &from);

private:
    /// The color map to use
    NIVissimLoader::ColorMap &myColorMap;
};


#endif

/****************************************************************************/

