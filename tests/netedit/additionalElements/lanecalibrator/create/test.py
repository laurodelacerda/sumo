#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2018 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    test.py
# @author  Pablo Alvarez Lopez
# @date    2016-11-25
# @version $Id$

# import common functions for netedit tests
import os
import sys

testRoot = os.path.join(os.environ.get('SUMO_HOME', '.'), 'tests')
neteditTestRoot = os.path.join(
    os.environ.get('TEXTTEST_HOME', testRoot), 'netedit')
sys.path.append(neteditTestRoot)
import neteditTestFunctions as netedit  # noqa

# Open netedit
neteditProcess, referencePosition = netedit.setupAndStart(neteditTestRoot, ['--gui-testing-debug-gl'])

# go to additional mode
netedit.additionalMode()

# select calibrator
netedit.changeAdditional("laneCalibrator")

# create calibrator
netedit.leftClick(referencePosition, 240, 250)

# change position with an invalid value (dummy)
netedit.modifyAdditionalDefaultValue(2, "dummyValue")

# create calibrator with an invalid parameter (Default value will be used)
netedit.leftClick(referencePosition, 400, 215)

# change position with an invalid value (negative)
netedit.modifyAdditionalDefaultValue(2, "-5")

# create calibrator with an invalid parameter (Default value will be used)
netedit.leftClick(referencePosition, 400, 215)

# change position with a valid value
netedit.modifyAdditionalDefaultValue(2, "10")

# create calibrator with an invalid parameter (Default value will be used)
netedit.leftClick(referencePosition, 400, 215)

# change frequency with an invalid value (dummy)
netedit.modifyAdditionalDefaultValue(3, "dummyValue")

# create calibrator with an invalid parameter (Default value will be used)
netedit.leftClick(referencePosition, 400, 180)

# change frequency with an invalid value (negative)
netedit.modifyAdditionalDefaultValue(3, "-30")

# create calibrator with an invalid parameter (Default value will be used)
netedit.leftClick(referencePosition, 400, 180)

# change frequency with a valid value
netedit.modifyAdditionalDefaultValue(3, "250")

# create calibrator with a valid parameter in other lane
netedit.leftClick(referencePosition, 400, 180)

# set invalid name
netedit.modifyAdditionalDefaultValue(4, "\"\"\"")

# try to create Calibrator with invalid name
netedit.leftClick(referencePosition, 200, 230)

# set valid name
netedit.modifyAdditionalDefaultValue(4, "customName")

# create Calibrator with valid name
netedit.leftClick(referencePosition, 200, 230)

# change routeprobe with a invalid routeProbe
netedit.modifyAdditionalDefaultValue(5, ";;;;;%%;;;;")

# create calibrator with a different routeProbe in other lane
netedit.leftClick(referencePosition, 200, 230)

# change routeprobe with a different routeProbe (valid, empty)
netedit.modifyAdditionalDefaultValue(5, "")

# create calibrator with a valid parameter in other lane
netedit.leftClick(referencePosition, 200, 230)

# change routeprobe with a different routeProbe
netedit.modifyAdditionalDefaultValue(5, "routeProbe")

# create calibrator with a valid parameter in other lane
netedit.leftClick(referencePosition, 200, 230)

# change output with an invalid value
netedit.modifyAdditionalDefaultValue(6, "%%%%##;;#!!!")

# create calibrator with a valid parameter in other lane
netedit.leftClick(referencePosition, 200, 230)

# change output with an valid value
netedit.modifyAdditionalDefaultValue(7, "myOwnOutput")

# create calibrator with a valid parameter in other lane
netedit.leftClick(referencePosition, 200, 230)

# Check undo redo
netedit.undo(referencePosition, 7)
netedit.redo(referencePosition, 7)

# save additionals
netedit.saveAdditionals()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
