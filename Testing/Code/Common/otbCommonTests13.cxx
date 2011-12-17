/*=========================================================================

  Program:   ORFEO Toolbox
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
  See OTBCopyright.txt for details.


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

// this file defines the otbCommonTest for the test driver
// and all it expects is that you have a function called RegisterTests


#include "otbTestMain.h"

void RegisterTests()
{
  REGISTER_TEST(otbPipelineMemoryPrintCalculatorNew);
  REGISTER_TEST(otbPipelineMemoryPrintCalculatorTest);
  REGISTER_TEST(otbStreamingManagerNew);
  REGISTER_TEST(otbNumberOfLinesStrippedStreamingManager);
  REGISTER_TEST(otbRAMDrivenStrippedStreamingManager);
  REGISTER_TEST(otbTileDimensionTiledStreamingManager);
  REGISTER_TEST(otbRAMDrivenTiledStreamingManager);
  REGISTER_TEST(otbImageRegionAdaptativeSplitterNew);
  REGISTER_TEST(otbImageRegionAdaptativeSplitter);
}
