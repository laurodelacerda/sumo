/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNETAZFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2018
/// @version $Id$
///
// The Widget for add TAZ elements
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <iostream>
#include <utils/foxtools/fxexdefs.h>
#include <utils/foxtools/MFXUtils.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <netedit/GNEViewParent.h>
#include <netedit/GNENet.h>
#include <netedit/changes/GNEChange_Additional.h>
#include <netedit/netelements/GNEJunction.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNELane.h>
#include <netedit/additionals/GNETAZ.h>
#include <netedit/additionals/GNETAZSink.h>
#include <netedit/additionals/GNETAZSource.h>
#include <netedit/additionals/GNEAdditionalHandler.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEAttributeCarrier.h>

#include "GNETAZFrame.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNETAZFrame::TAZParameters) TAZParametersMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SET_ATTRIBUTE_DIALOG,    GNETAZFrame::TAZParameters::onCmdSetColorAttribute),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SET_ATTRIBUTE,           GNETAZFrame::TAZParameters::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND, MID_HELP,                        GNETAZFrame::TAZParameters::onCmdHelp),
};

FXDEFMAP(GNETAZFrame::TAZSaveEdges) TAZSaveEdgesMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_OK,         GNETAZFrame::TAZSaveEdges::onCmdSaveChanges),
    FXMAPFUNC(SEL_COMMAND,  MID_CANCEL,     GNETAZFrame::TAZSaveEdges::onCmdCancelChanges),
};

FXDEFMAP(GNETAZFrame::TAZEdgesCommonParameters) TAZEdgesCommonParametersMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNETAZFrame::TAZEdgesCommonParameters::onCmdSetDefaultValues),
};

FXDEFMAP(GNETAZFrame::TAZEdgesGraphic) TAZEdgesGraphicMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_OPERATION,  GNETAZFrame::TAZEdgesGraphic::onCmdChoosenBy),
};

// Object implementation
FXIMPLEMENT(GNETAZFrame::TAZParameters,             FXGroupBox,     TAZParametersMap,               ARRAYNUMBER(TAZParametersMap))
FXIMPLEMENT(GNETAZFrame::TAZSaveEdges,              FXGroupBox,     TAZSaveEdgesMap,                ARRAYNUMBER(TAZSaveEdgesMap))
FXIMPLEMENT(GNETAZFrame::TAZEdgesCommonParameters,  FXGroupBox,     TAZEdgesCommonParametersMap,    ARRAYNUMBER(TAZEdgesCommonParametersMap))
FXIMPLEMENT(GNETAZFrame::TAZEdgesGraphic,           FXGroupBox,     TAZEdgesGraphicMap,             ARRAYNUMBER(TAZEdgesGraphicMap))


// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNETAZFrame::TAZCurrent - methods
// ---------------------------------------------------------------------------

GNETAZFrame::TAZCurrent::TAZCurrent(GNETAZFrame* TAZFrameParent) : 
    FXGroupBox(TAZFrameParent->myContentFrame, "TAZ", GUIDesignGroupBoxFrame),
    myTAZFrameParent(TAZFrameParent),
    myTAZCurrent(nullptr) {
    // create TAZ label
    myTAZCurrentLabel = new FXLabel(this, "No TAZ selected", 0, GUIDesignLabelLeft);
}


GNETAZFrame::TAZCurrent::~TAZCurrent() {}


void 
GNETAZFrame::TAZCurrent::setTAZ(GNETAZ* TAZCurrent) {
    // set new current TAZ
    myTAZCurrent = TAZCurrent;
    // update label and moduls
    if(myTAZCurrent != nullptr) {
        myTAZCurrentLabel->setText((("Current TAZ: ") + myTAZCurrent->getID()).c_str());
        // obtain a copy of all edges of the net (to avoid slowdown during manipulations
        myNetEdges = myTAZFrameParent->getViewNet()->getNet()->retrieveEdges();
        // obtain all edges of TAZ
        myTAZEdges.clear();
        for (const auto &i : myTAZCurrent->getAdditionalChilds()) {
            myTAZEdges.insert(myTAZFrameParent->getViewNet()->getNet()->retrieveEdge(i->getAttribute(SUMO_ATTR_EDGE)));
        }
        // hide TAZ parameters
        myTAZFrameParent->myTAZParameters->hideTAZParametersModul();
        // hide Netedit parameters
        myTAZFrameParent->myNeteditAttributes->hideNeteditAttributesModul();
        // hide drawing shape
        myTAZFrameParent->myDrawingShape->hideDrawingShape();
        // show save TAZ Edges
        myTAZFrameParent->myTAZSaveEdges->showTAZSaveEdgesModul();
        // show edge common parameters
        myTAZFrameParent->myTAZEdgesCommonParameters->showTAZEdgesCommonParametersModul();
        // show edge common parameters
        myTAZFrameParent->myTAZCommonStatistics->showTAZCommonStatisticsModul();
        // show edge Selection parameters
        myTAZFrameParent->myTAZSelectionStatistics->showTAZSelectionStatisticsModul();
        // show Edges graphics
        myTAZFrameParent->myTAZEdgesGraphic->showTAZEdgesGraphicModul();
    } else {
        // show TAZ parameters
        myTAZFrameParent->myTAZParameters->showTAZParametersModul();
        // show Netedit parameters
        myTAZFrameParent->myNeteditAttributes->showNeteditAttributesModul(GNEAttributeCarrier::getTagProperties(SUMO_TAG_TAZ));
        // show drawing shape
        myTAZFrameParent->myDrawingShape->showDrawingShape();
        // hide edge common parameters
        myTAZFrameParent->myTAZEdgesCommonParameters->hideTAZEdgesCommonParametersModul();
        // hide edge common parameters
        myTAZFrameParent->myTAZCommonStatistics->hideTAZCommonStatisticsModul();
        // hide edge Selection parameters
        myTAZFrameParent->myTAZSelectionStatistics->hideTAZSelectionStatisticsModul();
        // hide Edges graphics
        myTAZFrameParent->myTAZEdgesGraphic->hideTAZEdgesGraphicModul();
        // hide save TAZ Edges
        myTAZFrameParent->myTAZSaveEdges->hideTAZSaveEdgesModul();
        // restore label
        myTAZCurrentLabel->setText("No TAZ selected");
        // clear net edges (always the last step due hideTAZEdgesGraphicModul() function)
        myNetEdges.clear();
    }
}


GNETAZ* 
GNETAZFrame::TAZCurrent::getTAZ() const {
    return myTAZCurrent;
}


bool 
GNETAZFrame::TAZCurrent::isTAZEdge(GNEEdge* edge) const {
    return myTAZEdges.find(edge) != myTAZEdges.end();
}


const std::vector<GNEEdge*>&
GNETAZFrame::TAZCurrent::getNetEdges() const {
    return myNetEdges;
}

// ---------------------------------------------------------------------------
// GNETAZFrame::TAZSaveEdges - methods
// ---------------------------------------------------------------------------

GNETAZFrame::TAZSaveEdges::TAZSaveEdges(GNETAZFrame* TAZFrameParent) : 
    FXGroupBox(TAZFrameParent->myContentFrame, "Save TAZ Edges", GUIDesignGroupBoxFrame),
    myTAZFrameParent(TAZFrameParent) {
    // Create groupbox for save changes
    mySaveChangesButton = new FXButton(this, "Save changes", GUIIconSubSys::getIcon(ICON_SAVE), this, MID_OK, GUIDesignButton);
    mySaveChangesButton->disable();
    // Create groupbox cancel changes
    myCancelChangesButton = new FXButton(this, "Sancel changes", GUIIconSubSys::getIcon(ICON_CANCEL), this, MID_CANCEL, GUIDesignButton);
    myCancelChangesButton->disable();
    // Create groupbox and label for legend
    FXLabel *colorSelectedLabel = new FXLabel(this, "Edge Selected", 0, GUIDesignLabelLeft);
    colorSelectedLabel->setBackColor(MFXUtils::getFXColor(TAZFrameParent->getEdgeCandidateSelectedColor()));
}


GNETAZFrame::TAZSaveEdges::~TAZSaveEdges() {}


void 
GNETAZFrame::TAZSaveEdges::showTAZSaveEdgesModul() {
    show();
}


void 
GNETAZFrame::TAZSaveEdges::hideTAZSaveEdgesModul() {
    // cancel changes
    onCmdCancelChanges(0,0,0);
    hide();
}


void
GNETAZFrame::TAZSaveEdges::enableButtons() {
    // enable mySaveChangesButton and myCancelChangesButton, and start a undolist set
    if (!mySaveChangesButton->isEnabled()) {
        mySaveChangesButton->enable();
        myCancelChangesButton->enable();
        myTAZFrameParent->myViewNet->getUndoList()->p_begin("TAZ attributes");
    }
}

long
GNETAZFrame::TAZSaveEdges::onCmdSaveChanges(FXObject*, FXSelector, void*) {
    // disable mySaveChangesButton and myCancelChangesButtonand, and finish undolist set
    if (mySaveChangesButton->isEnabled()) {
        mySaveChangesButton->disable();
        myCancelChangesButton->disable();
        myTAZFrameParent->myViewNet->getUndoList()->p_end();
    }
    return 1;
}


long
GNETAZFrame::TAZSaveEdges::onCmdCancelChanges(FXObject*, FXSelector, void*) {
    // cancel changes and disable buttons
    if (mySaveChangesButton->isEnabled()) {
        myTAZFrameParent->myViewNet->getUndoList()->p_abort();
        mySaveChangesButton->disable();
        myCancelChangesButton->disable();
    }
    return 1;
}

// ---------------------------------------------------------------------------
// GNETAZFrame::TAZEdgesCommonParameters - methods
// ---------------------------------------------------------------------------

GNETAZFrame::TAZEdgesCommonParameters::TAZEdgesCommonParameters(GNETAZFrame* TAZFrameParent) : 
    FXGroupBox(TAZFrameParent->myContentFrame, "Default values", GUIDesignGroupBoxFrame),
    myTAZFrameParent(TAZFrameParent),
    myDefaultTAZSourceWeight(1),
    myDefaultTAZSinkWeight(1) {
    // create checkbox for toogle membership
    FXHorizontalFrame* toogleMembershipFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(toogleMembershipFrame, "Membership", 0, GUIDesignLabelAttribute);
    myToggleMembership = new FXCheckButton(toogleMembershipFrame, "Toggle", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButtonAttribute);
    // by default enabled
    myToggleMembership->setCheck(TRUE);
    // create default TAZ Source weight
    FXHorizontalFrame* defaultTAZSourcesFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myCheckBoxSetDefaultValueTAZSources = new FXCheckButton(defaultTAZSourcesFrame, "Def. source", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButtonAttributeLabel);
    myTextFieldDefaultValueTAZSources = new FXTextField(defaultTAZSourcesFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldReal);
    myTextFieldDefaultValueTAZSources->setText("1");
    // by default disable
    myCheckBoxSetDefaultValueTAZSources->setCheck(FALSE);
    myTextFieldDefaultValueTAZSources->disable();
    // create default TAZ Sink weight
    FXHorizontalFrame* defaultTAZSinksFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myCheckBoxSetDefaultValueTAZSinks = new FXCheckButton(defaultTAZSinksFrame, "Def. sink", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButtonAttributeLabel);
    myTextFieldDefaultValueTAZSinks = new FXTextField(defaultTAZSinksFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldReal);
    myTextFieldDefaultValueTAZSinks->setText("1");
    // by default disable
    myCheckBoxSetDefaultValueTAZSinks->setCheck(FALSE);
    myTextFieldDefaultValueTAZSinks->disable();
}


GNETAZFrame::TAZEdgesCommonParameters::~TAZEdgesCommonParameters() {}


void 
GNETAZFrame::TAZEdgesCommonParameters::showTAZEdgesCommonParametersModul() {
    show();
}


void 
GNETAZFrame::TAZEdgesCommonParameters::hideTAZEdgesCommonParametersModul() {
    hide();
}


double 
GNETAZFrame::TAZEdgesCommonParameters::getDefaultTAZSourceWeight() const {
    if (myCheckBoxSetDefaultValueTAZSources->getCheck() == TRUE) {
        return myDefaultTAZSourceWeight;
    } else {
        return 1;
    }
}


double 
GNETAZFrame::TAZEdgesCommonParameters::getDefaultTAZSinkWeight() const {
    if (myCheckBoxSetDefaultValueTAZSinks->getCheck() == TRUE) {
        return myDefaultTAZSinkWeight;
    } else {
        return 1;
    }
}


long
GNETAZFrame::TAZEdgesCommonParameters::onCmdSetDefaultValues(FXObject* obj, FXSelector, void*) {
    // find object
    if(obj == myToggleMembership) {
        // set text of myToggleMembership
        if (myToggleMembership->getCheck() == TRUE) {
            myToggleMembership->setText("toogle");
        } else {
            myToggleMembership->setText("keep");
        }
    } else if (obj == myCheckBoxSetDefaultValueTAZSources) {
        // enable or disable myTextFieldDefaultValueTAZSources
        if (myCheckBoxSetDefaultValueTAZSources->getCheck() == TRUE) {
            myTextFieldDefaultValueTAZSources->enable();
        } else {
            myTextFieldDefaultValueTAZSources->disable();
        }
    } else if (obj == myTextFieldDefaultValueTAZSources) {
        if (GNEAttributeCarrier::canParse<double>(myTextFieldDefaultValueTAZSources->getText().text())) {
            myDefaultTAZSourceWeight = GNEAttributeCarrier::parse<double>(myTextFieldDefaultValueTAZSources->getText().text());
            // check if myDefaultTAZSourceWeight is greather than 0
            if (myDefaultTAZSourceWeight >= 0) {
                // set valid color
                myTextFieldDefaultValueTAZSources->setTextColor(FXRGB(0, 0, 0));
            } else {
                // set invalid color
                myTextFieldDefaultValueTAZSources->setTextColor(FXRGB(255, 0, 0));
                myDefaultTAZSourceWeight = 1;
            }
        } else {
            // set invalid color
            myTextFieldDefaultValueTAZSources->setTextColor(FXRGB(255, 0, 0));
            myDefaultTAZSourceWeight = 1;
        }
    } else if (obj == myCheckBoxSetDefaultValueTAZSinks) {
        // enable or disable myTextFieldDefaultValueTAZSources
        if (myCheckBoxSetDefaultValueTAZSinks->getCheck() == TRUE) {
            myTextFieldDefaultValueTAZSinks->enable();
        } else {
            myTextFieldDefaultValueTAZSinks->disable();
        }
    } else if (obj == myTextFieldDefaultValueTAZSinks) {
        if (GNEAttributeCarrier::canParse<double>(myTextFieldDefaultValueTAZSinks->getText().text())) {
            myDefaultTAZSinkWeight = GNEAttributeCarrier::parse<double>(myTextFieldDefaultValueTAZSinks->getText().text());
            // check if myDefaultTAZSinkWeight is greather than 0
            if (myDefaultTAZSinkWeight >= 0) {
                // set valid color
                myTextFieldDefaultValueTAZSinks->setTextColor(FXRGB(0, 0, 0));
            } else {
                // set invalid color
                myTextFieldDefaultValueTAZSinks->setTextColor(FXRGB(255, 0, 0));
                myDefaultTAZSinkWeight = 1;
            }
        } else {
            // set invalid color
            myTextFieldDefaultValueTAZSinks->setTextColor(FXRGB(255, 0, 0));
            myDefaultTAZSinkWeight = 1;
        }
    }
    return 1;
}

// ---------------------------------------------------------------------------
// GNETAZFrame::TAZCommonStatistics - methods
// ---------------------------------------------------------------------------

GNETAZFrame::TAZCommonStatistics::TAZCommonStatistics(GNETAZFrame* TAZFrameParent) : 
    FXGroupBox(TAZFrameParent->myContentFrame, "TAZ Statistics", GUIDesignGroupBoxFrame),
    myTAZFrameParent(TAZFrameParent) {
    // create label for statistics
    myStatisticsLabel = new FXLabel(this, "Statistics", 0, GUIDesignLabelFrameInformation);
}


GNETAZFrame::TAZCommonStatistics::~TAZCommonStatistics() {}


void 
GNETAZFrame::TAZCommonStatistics::showTAZCommonStatisticsModul() {
    // always update statistics after show
    updateStatistics();
    show();
}


void 
GNETAZFrame::TAZCommonStatistics::hideTAZCommonStatisticsModul() {
    hide();
}


void 
GNETAZFrame::TAZCommonStatistics::updateStatistics() {
    if (myTAZFrameParent->myTAZCurrent->getTAZ()) {
        // declare ostringstream for statistics
        std::ostringstream information;
        information
            << "- Number of Edges: " << toString(myTAZFrameParent->myTAZCurrent->getTAZ()->getAdditionalChilds().size()/2) << "\n"
            << "- Min source: " << myTAZFrameParent->myTAZCurrent->getTAZ()->getAttribute(GNE_ATTR_MIN_SOURCE) << "\n"
            << "- Max source: " << myTAZFrameParent->myTAZCurrent->getTAZ()->getAttribute(GNE_ATTR_MAX_SOURCE) << "\n"
            << "- Average source: " << myTAZFrameParent->myTAZCurrent->getTAZ()->getAttribute(GNE_ATTR_AVERAGE_SOURCE) << "\n"
            << "\n"
            << "- Min sink: " << myTAZFrameParent->myTAZCurrent->getTAZ()->getAttribute(GNE_ATTR_MIN_SINK) << "\n"
            << "- Max sink: " << myTAZFrameParent->myTAZCurrent->getTAZ()->getAttribute(GNE_ATTR_MAX_SINK) << "\n"
            << "- Average sink: " << myTAZFrameParent->myTAZCurrent->getTAZ()->getAttribute(GNE_ATTR_AVERAGE_SINK);
        // set new label
        myStatisticsLabel->setText(information.str().c_str());
    } else {
        myStatisticsLabel->setText("No TAZ Selected");
    }
}

// ---------------------------------------------------------------------------
// GNETAZFrame::TAZSelectionStatistics - methods
// ---------------------------------------------------------------------------

GNETAZFrame::TAZSelectionStatistics::TAZSelectionStatistics(GNETAZFrame* TAZFrameParent) : 
    FXGroupBox(TAZFrameParent->myContentFrame, "Selection Statistics", GUIDesignGroupBoxFrame),
    myTAZFrameParent(TAZFrameParent) {
    // create label for statistics
    myStatisticsLabel = new FXLabel(this, "Statistics", 0, GUIDesignLabelFrameInformation);
}


GNETAZFrame::TAZSelectionStatistics::~TAZSelectionStatistics() {}


void 
GNETAZFrame::TAZSelectionStatistics::showTAZSelectionStatisticsModul() {
    // update Statistics before show
    updateStatistics();
    show();
}


void 
GNETAZFrame::TAZSelectionStatistics::hideTAZSelectionStatisticsModul() {
    // clear childs before hide
    clearSelectedEdges();
    hide();
}


void 
GNETAZFrame::TAZSelectionStatistics::selectEdge(GNEEdge* edge, GNEAdditional* TAZSource, GNEAdditional* TAZSink) {
    if(edge && TAZSource && TAZSink) {
        if (myTAZChildSelected.count(edge) == 0) {
            // add edge and their TAZ Childs into myTAZChildSelected
            myTAZChildSelected[edge] = std::make_pair(TAZSource, TAZSink);
            // always update statistics after insertion
            updateStatistics();
            // update edge colors
            myTAZFrameParent->myTAZEdgesGraphic->updateEdgeColors();
        } else {
            throw ProcessError("edge already selected");
        }
    } else {
        throw ProcessError("Invalid edge or TAZ Childs");
    }
}


void 
GNETAZFrame::TAZSelectionStatistics::unselectEdge(GNEEdge* edge) {
    if(edge) {
        if (myTAZChildSelected.count(edge) ==1) {
            // remove edge (their TAZ Childs will be automatic removed)
            myTAZChildSelected.erase(edge);
            // always update statistics after insertion
            updateStatistics();
            // update edge colors
            myTAZFrameParent->myTAZEdgesGraphic->updateEdgeColors();
        } else {
            throw ProcessError("edge isn't selected");
        }
    } else {
        throw ProcessError("Invalid edge");
    }
}


bool 
GNETAZFrame::TAZSelectionStatistics::isEdgeSelected(GNEEdge* edge) {
    return (myTAZChildSelected.count(edge) != 0);
}


bool 
GNETAZFrame::TAZSelectionStatistics::edgesSelected() const {
    return (myTAZChildSelected.size() > 0);
}


void 
GNETAZFrame::TAZSelectionStatistics::clearSelectedEdges() {
    // clear all selected edges (and the TAZ Childs)
    myTAZChildSelected.clear();
    // always update statistics after clear edges
    updateStatistics();
    // update edge colors
    myTAZFrameParent->myTAZEdgesGraphic->updateEdgeColors();
}


void 
GNETAZFrame::TAZSelectionStatistics::updateStatistics() {
    if (myTAZChildSelected.size() > 0) {
        // declare stadistic variables
        double weight = 0;
        double maxWeightSource = 0;
        double minWeightSource = -1;
        double averageWeightSource = 0;
        double maxWeightSink = 0;
        double minWeightSink = -1;
        double averageWeightSink = 0;
        // iterate over additional childs
        for (auto i : myTAZChildSelected) {
            //start with sources
            weight = GNEAttributeCarrier::parse<double>(i.second.first->getAttribute(SUMO_ATTR_WEIGHT));
            // check max Weight
            if (maxWeightSource < weight) {
                maxWeightSource = weight;
            }
            // check min Weight
            if (minWeightSource == -1 || (maxWeightSource < weight)) {
                minWeightSource = weight;
            }
            // update Average
            averageWeightSource += weight;
            // continue with sinks
            weight = GNEAttributeCarrier::parse<double>(i.second.second->getAttribute(SUMO_ATTR_WEIGHT));
            // check max Weight
            if (maxWeightSink < weight) {
                maxWeightSink = weight;
            }
            // check min Weight
            if (minWeightSink == -1 || (maxWeightSink < weight)) {
                minWeightSink = weight;
            }
            // update Average
            averageWeightSink += weight;
        }
        // calculate average
        averageWeightSource /= myTAZChildSelected.size();
        averageWeightSink /= myTAZChildSelected.size();
        // declare ostringstream for statistics
        std::ostringstream information;
        std::string edgeInformation;
        // first fill edgeInformation
        if(myTAZChildSelected.size() == 1) {
            edgeInformation = "- Edge ID: " + myTAZChildSelected.begin()->first->getID();
        } else {
            edgeInformation = "- Number of edges: " + toString(myTAZChildSelected.size());
        }
        // fill rest of information
        information
            << edgeInformation << "\n"
            << "- Min source: " << toString(minWeightSource) << "\n"
            << "- Max source: " << toString(maxWeightSource) << "\n"
            << "- Average source: " << toString(averageWeightSource) << "\n"
            << "\n"
            << "- Min sink: " << toString(minWeightSink) << "\n"
            << "- Max sink: " << toString(maxWeightSink) << "\n"
            << "- Average sink: " << toString(averageWeightSink);
        // set new label
        myStatisticsLabel->setText(information.str().c_str());
    } else {
        myStatisticsLabel->setText("No edge selected");
    }
}

// ---------------------------------------------------------------------------
// GNETAZFrame::TAZParameters- methods
// ---------------------------------------------------------------------------

GNETAZFrame::TAZParameters::TAZParameters(GNETAZFrame* TAZFrameParent) :
    FXGroupBox(TAZFrameParent->myContentFrame, "TAZ parameters", GUIDesignGroupBoxFrame),
    myTAZFrameParent(TAZFrameParent) {
    // create Button and string textField for color and set blue as default color
    FXHorizontalFrame* colorParameter = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myColorEditor = new FXButton(colorParameter, toString(SUMO_ATTR_COLOR).c_str(), 0, this, MID_GNE_SET_ATTRIBUTE_DIALOG, GUIDesignButtonAttribute);
    myTextFieldColor = new FXTextField(colorParameter, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myTextFieldColor->setText("blue");
    // create Label and CheckButton for use innen edges with true as default value
    FXHorizontalFrame* useInnenEdges = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(useInnenEdges, "Edges within", 0, GUIDesignLabelAttribute);
    myAddEdgesWithinCheckButton = new FXCheckButton(useInnenEdges, "use", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButtonAttribute);
    myAddEdgesWithinCheckButton->setCheck(true);
    // Create help button
    myHelpTAZAttribute = new FXButton(this, "Help", 0, this, MID_HELP, GUIDesignButtonRectangular);
}


GNETAZFrame::TAZParameters::~TAZParameters() {}


void
GNETAZFrame::TAZParameters::showTAZParametersModul() {
    FXGroupBox::show();
}


void
GNETAZFrame::TAZParameters::hideTAZParametersModul() {
    FXGroupBox::hide();
}


bool
GNETAZFrame::TAZParameters::isCurrentParametersValid() const {
    return GNEAttributeCarrier::canParse<RGBColor>(myTextFieldColor->getText().text());
}


bool 
GNETAZFrame::TAZParameters::isAddEdgesWithinEnabled() const {
    return (myAddEdgesWithinCheckButton->getCheck() == TRUE);
}


std::map<SumoXMLAttr, std::string> 
GNETAZFrame::TAZParameters::getAttributesAndValues() const {
    std::map<SumoXMLAttr, std::string> parametersAndValues;
    // get color (currently the only editable attribute)
    parametersAndValues[SUMO_ATTR_COLOR] = myTextFieldColor->getText().text();
    return parametersAndValues;
}


long 
GNETAZFrame::TAZParameters::onCmdSetColorAttribute(FXObject*, FXSelector, void*) {
    // create FXColorDialog
    FXColorDialog colordialog(this, tr("Color Dialog"));
    colordialog.setTarget(this);
    // If previous attribute wasn't correct, set black as default color
    if (GNEAttributeCarrier::canParse<RGBColor>(myTextFieldColor->getText().text())) {
        colordialog.setRGBA(MFXUtils::getFXColor(RGBColor::parseColor(myTextFieldColor->getText().text())));
    } else {
        colordialog.setRGBA(MFXUtils::getFXColor(RGBColor::parseColor("blue")));
    }
    // execute dialog to get a new color
    if (colordialog.execute()) {
        myTextFieldColor->setText(toString(MFXUtils::getRGBColor(colordialog.getRGBA())).c_str());
        onCmdSetAttribute(0, 0, 0);
    }
    return 0;
}


long
GNETAZFrame::TAZParameters::onCmdSetAttribute(FXObject*, FXSelector, void*) {
    // only COLOR text field has to be checked
    bool currentParametersValid = GNEAttributeCarrier::canParse<RGBColor>(myTextFieldColor->getText().text());
    // change color of textfield dependig of myCurrentParametersValid
    if (currentParametersValid) {
        myTextFieldColor->setTextColor(FXRGB(0, 0, 0));
        myTextFieldColor->killFocus();
    } else {
        myTextFieldColor->setTextColor(FXRGB(255, 0, 0));
        currentParametersValid = false;
    }
    // change useInnenEdgesCheckButton text
    if(myAddEdgesWithinCheckButton->getCheck() == TRUE) {
        myAddEdgesWithinCheckButton->setText("use");
    } else {
        myAddEdgesWithinCheckButton->setText("not use");
    }
    return 0;
}


long
GNETAZFrame::TAZParameters::onCmdHelp(FXObject*, FXSelector, void*) {
    myTAZFrameParent->openHelpAttributesDialog(GNEAttributeCarrier::getTagProperties(SUMO_TAG_TAZ));
    return 1;
}

// ---------------------------------------------------------------------------
// GNETAZFrame::TAZEdgesGraphic - methods
// ---------------------------------------------------------------------------

GNETAZFrame::TAZEdgesGraphic::TAZEdgesGraphic(GNETAZFrame* TAZFrameParent) : 
    FXGroupBox(TAZFrameParent->myContentFrame, "Edges", GUIDesignGroupBoxFrame),
    myTAZFrameParent(TAZFrameParent) {
    // create Radio button for show edges by source weight
    myColorBySourceWeight = new FXRadioButton(this, "Color by Source", this, MID_CHOOSEN_OPERATION, GUIDesignRadioButton);
    // create Radio button for show edges by sink weight
    myColorBySinkWeight = new FXRadioButton(this, "Color by Sink", this, MID_CHOOSEN_OPERATION, GUIDesignRadioButton);
    // create Radio button for show edges by source and sink weight
    myColorBySourceAndSinkWeight = new FXRadioButton(this, "Color by Source+Sink", this, MID_CHOOSEN_OPERATION, GUIDesignRadioButton);
    // show by source as default
    myColorBySourceWeight->setCheck(true);
}


GNETAZFrame::TAZEdgesGraphic::~TAZEdgesGraphic() {}


void 
GNETAZFrame::TAZEdgesGraphic::showTAZEdgesGraphicModul() {
    // update edge colors
    updateEdgeColors();
    show();
}


void 
GNETAZFrame::TAZEdgesGraphic::hideTAZEdgesGraphicModul() {
    // iterate over all edges and restore color
    for (const auto &i : myTAZFrameParent->myTAZCurrent->getNetEdges()) {
        for (const auto j : i->getLanes() ) {
            j->setSpecialColor(nullptr);
        }
    }
    hide();
}


void 
GNETAZFrame::TAZEdgesGraphic::updateEdgeColors() {
    // iterate over all edges and set gray color
    for (const auto &i : myTAZFrameParent->myTAZCurrent->getNetEdges()) {
        if(myTAZFrameParent->myTAZSelectionStatistics->isEdgeSelected(i)) {
            // set candidate selected color
            for (const auto j : i->getLanes() ) {
                j->setSpecialColor(&myTAZFrameParent->getEdgeCandidateSelectedColor());
            }
        } else if (myTAZFrameParent->myTAZCurrent->isTAZEdge(i)) {
            // set candidate color
            for (const auto j : i->getLanes() ) {
                j->setSpecialColor(&myTAZFrameParent->getEdgeCandidateColor());
            }
        } else {
            // set candidate color (in this case, 
            for (const auto j : i->getLanes() ) {
                j->setSpecialColor(&RGBColor::GREY);
            }
        }
    }
}


long
GNETAZFrame::TAZEdgesGraphic::onCmdChoosenBy(FXObject* obj, FXSelector, void*) {
    // check what radio was pressed and disable the others
    if (obj == myColorBySourceWeight) {
        myColorBySinkWeight->setCheck(FALSE);
        myColorBySourceAndSinkWeight->setCheck(FALSE);
    } else if (obj == myColorBySinkWeight) {
        myColorBySourceWeight->setCheck(FALSE);
        myColorBySourceAndSinkWeight->setCheck(FALSE);
    } else if (obj == myColorBySourceAndSinkWeight) {
        myColorBySourceWeight->setCheck(FALSE);
        myColorBySinkWeight->setCheck(FALSE);
    }
    // update edge colors
    updateEdgeColors();
    return 1;
}

// ---------------------------------------------------------------------------
// GNETAZFrame - methods
// ---------------------------------------------------------------------------

GNETAZFrame::GNETAZFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet) :
    GNEFrame(horizontalFrameParent, viewNet, "TAZs") {

    // create current TAZ modul
    myTAZCurrent = new TAZCurrent(this);

    // Create TAZ Parameters modul
    myTAZParameters = new TAZParameters(this);

    /// @brief create  Netedit parameter
    myNeteditAttributes = new NeteditAttributes(this);

    // Create drawing controls modul
    myDrawingShape = new DrawingShape(this);

    // Create save TAZ Edges modul
    myTAZSaveEdges = new TAZSaveEdges(this);

    // Create TAZ Edges Common Parameters modul
    myTAZEdgesCommonParameters = new TAZEdgesCommonParameters(this);

    // Create TAZ Edges Common Statistics modul
    myTAZCommonStatistics = new TAZCommonStatistics(this);

    // Create TAZ Edges Selection Statistics modul
    myTAZSelectionStatistics = new TAZSelectionStatistics(this);

    // Create TAZ Edges Common Parameters modul
    myTAZEdgesGraphic = new TAZEdgesGraphic(this);

    // by default there isn't a TAZ
    myTAZCurrent->setTAZ(nullptr);
}


GNETAZFrame::~GNETAZFrame() {
}


void
GNETAZFrame::hide() {
    // hide frame
    GNEFrame::hide();
}


bool 
GNETAZFrame::processClick(const Position& clickedPosition, GNETAZ *TAZ, GNEEdge* edge) {
    // Declare map to keep values
    std::map<SumoXMLAttr, std::string> valuesOfElement;
    if (myDrawingShape->isDrawing()) {
        // add or delete a new point depending of flag "delete last created point"
        if (myDrawingShape->getDeleteLastCreatedPoint()) {
            myDrawingShape->removeLastPoint();
        } else {
            myDrawingShape->addNewPoint(clickedPosition);
        }
        return true;
    } else if (myTAZCurrent->getTAZ() == nullptr) {
        // avoid reset of Frame if user doesn't click over an TAZ
        if(TAZ) {
            myTAZCurrent->setTAZ(TAZ);
            return true;
        } else {
            return false;
        }
    } else if (edge != nullptr) {
        // declare variables to save source and sink childs
        GNEAdditional* TAZSource = nullptr;
        GNEAdditional* TAZSink = nullptr;
        for (const auto &i : myTAZCurrent->getTAZ()->getAdditionalChilds()) {
            // check if clicked edge correspond to TAZSource/sink's edge
            if (i->getAttribute(SUMO_ATTR_EDGE) == edge->getID()) {
                if(i->getTagProperty().getTag() == SUMO_TAG_TAZSOURCE) {
                    TAZSource = i;
                } else {
                    TAZSink = i;
                }
            }
        }
        // make sure that both  TAZSource and TAZSink exist befor adding it into myTAZSelectionStatistics
        if(edge && TAZSource && TAZSink) {
            // first clear current selection
            myTAZSelectionStatistics->clearSelectedEdges();
            // now select edge (and their Source/Sinks)
            myTAZSelectionStatistics->selectEdge(edge, TAZSource, TAZSink);
            return true;
        } else {
            return false;
        }
    } else {
        // nothing to do
        return false;
    }
}


void 
GNETAZFrame::processEdgeSelection(const std::vector<GNEEdge*>& edges) {
    // first check that a TAZ is selected
    if (myTAZCurrent->getTAZ()) {
        // iterate over edges
        for (auto i : edges) {
            // first check that selected edge isn't already selected
            if (!myTAZSelectionStatistics->isEdgeSelected(i)) {
                // declare variables to save source and sink childs
                GNEAdditional* TAZSource = nullptr;
                GNEAdditional* TAZSink = nullptr;
                for (const auto &j : myTAZCurrent->getTAZ()->getAdditionalChilds()) {
                    // check if selected edge correspond to TAZSource/sink's edge
                    if (j->getAttribute(SUMO_ATTR_EDGE) == i->getID()) {
                        if(j->getTagProperty().getTag() == SUMO_TAG_TAZSOURCE) {
                            TAZSource = j;
                        } else {
                            TAZSink = j;
                        }
                    }
                }
                // make sure that both  TAZSource and TAZSink exist befor adding it into myTAZSelectionStatistics
                if(TAZSource && TAZSink) {
                    // select edge (and their Source/Sinks)
                    myTAZSelectionStatistics->selectEdge(i, TAZSource, TAZSink);
                }
            }
        }
    }
}


GNETAZFrame::DrawingShape*
GNETAZFrame::getDrawingShape() const {
    return myDrawingShape;
}


GNETAZFrame::TAZCurrent* 
GNETAZFrame::getTAZCurrent() const {
    return myTAZCurrent;
}


GNETAZFrame::TAZSelectionStatistics* 
GNETAZFrame::getTAZSelectionStatistics() const {
    return myTAZSelectionStatistics;
}


bool
GNETAZFrame::buildShape() {
    // show warning dialogbox and stop check if input parameters are valid
    if (!myTAZParameters->isCurrentParametersValid()) {
        return false;
    } else if(myDrawingShape->getTemporalShape().size() == 0) {
        WRITE_WARNING("TAZ shape cannot be empty");
        return false;
    } else {
        // Declare map to keep TAZ Parameters values
        std::map<SumoXMLAttr, std::string> valuesOfElement = myTAZParameters->getAttributesAndValues();

        // obtain Netedit attributes
        myNeteditAttributes->getNeteditAttributesAndValues(valuesOfElement, nullptr);

        // generate new ID
        valuesOfElement[SUMO_ATTR_ID] = myViewNet->getNet()->generateAdditionalID(SUMO_TAG_TAZ);

        // obtain shape and close it
        PositionVector shape = myDrawingShape->getTemporalShape();
        shape.closePolygon();
        valuesOfElement[SUMO_ATTR_SHAPE] = toString(shape);

        // check if TAZ has to be created with edges
        if (myTAZParameters->isAddEdgesWithinEnabled()) {
            std::vector<std::string> edgeIDs;
            auto ACsInBoundary = myViewNet->getAttributeCarriersInBoundary(shape.getBoxBoundary(), true);
            for (auto i : ACsInBoundary) {
                if(i.second->getTagProperty().getTag() == SUMO_TAG_EDGE) {
                    edgeIDs.push_back(i.first);
                }
            }
            valuesOfElement[SUMO_ATTR_EDGES] = toString(edgeIDs);
        } else {
            // TAZ is created without edges
            valuesOfElement[SUMO_ATTR_EDGES] = "";
        }
        // return true if TAZ was successfully created
        return GNEAdditionalHandler::buildAdditional(myViewNet, true, SUMO_TAG_TAZ, valuesOfElement) != nullptr;
    }
}


void 
GNETAZFrame::enableModuls(const GNEAttributeCarrier::TagProperties &/*tagValue*/) {

}


void 
GNETAZFrame::disableModuls() {

}

/****************************************************************************/
