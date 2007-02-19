/****************************************************************************/
/// @file    OptionsSubSys.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 23.06.2003
/// @version $Id$
///
// missing_desc
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

#include <cassert>
#include <fstream>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsIO.h>
#include <utils/common/StringTokenizer.h>
#include "OptionsSubSys.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// static member definitions
// ===========================================================================
OptionsCont OptionsSubSys::myOptions;


// ===========================================================================
// method definitions
// ===========================================================================
bool
OptionsSubSys::init(bool loadConfig, int argc, char **argv,
                    fill_options *fill_f,
                    check_options *check_f)
{
    (*fill_f)(myOptions);
    // parse options
    if (OptionsIO::getOptions(loadConfig, &myOptions, argc, argv)) {
        // 01.06.2005: version retrieval patched
        // check the settings
        if (check_f!=0&&!check_f(myOptions)) {
            return false;
        }
    } else {
        // the options could not be parsed
        //  something is wrong with the calling parameter
        return false;
    }
    return true;
}


bool
OptionsSubSys::guiInit(fill_options *fill_f, const std::string &conf)
{
    (*fill_f)(myOptions);
    myOptions.set("configuration-file", conf);
    // parse options
    if (!OptionsIO::getOptions(true, &myOptions, 1, 0)) {
        // the options could not be parsed
        //  - something is wrong with the calling parameter
        return false;
    }
    return true;
}


OptionsCont &
OptionsSubSys::getOptions()
{
    return myOptions;
}


void
OptionsSubSys::close()
{
    myOptions.clear();
}


bool
OptionsSubSys::helper_CSVOptionMatches(const std::string &optionName,
                                       const std::string &itemName)
{
    if (myOptions.isSet(optionName)) {
        StringTokenizer st(myOptions.getString(optionName), ";");
        while (st.hasNext()) {
            if (st.next()==itemName) {
                return true;
            }
        }
    }
    return false;
}



/****************************************************************************/

