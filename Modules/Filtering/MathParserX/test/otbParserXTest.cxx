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
#include "otbMath.h"
#include "otbParserX.h"

typedef otb::ParserX ParserType;

int otbParserXTestNew(int itkNotUsed(argc), char * itkNotUsed(argv) [])
{
  // Instantiating object
  ParserType::Pointer parser = ParserType::New();
  std::cout << parser << std::endl;
  return EXIT_SUCCESS;
}

void otbParserXTest_ThrowIfNotEqual(double output, double ref, std::string testname, double epsilon = 1.0E-12)
{
  std::cout << "Running test " << testname << std::endl;
  if (vcl_abs(output-ref) > epsilon)
  {
    itkGenericExceptionMacro( << "Got " << output << " while waiting for " << ref );
  }
  std::cout << " -- OK" << std::endl;
}

void otbParserXTest_ThrowIfNotEqual(bool output, bool ref, std::string testname = "")
{
  std::cout << "Running test " << testname << std::endl;
  if (output != ref)
  {
    itkGenericExceptionMacro( << "Got " << output << " while waiting for " << ref );
  }
  std::cout << " -- OK" << std::endl;
}

void otbParserXTest_Numerical(void)
{
  ParserType::Pointer parser = ParserType::New();
  parser->SetExpr("10.0+3");
  otbParserXTest_ThrowIfNotEqual(parser->Eval(), 13.0, "Numerical");
}

void otbParserXTest_BuildInFun(void)
{
  ParserType::Pointer parser = ParserType::New();
  parser->SetExpr("cos(1.55)");
  otbParserXTest_ThrowIfNotEqual(parser->Eval(), vcl_cos(1.55), "BuildInFun");
}

void otbParserXTest_UserDefinedCst(void)
{
  ParserType::Pointer parser = ParserType::New();
  parser->SetExpr("ln10");
  otbParserXTest_ThrowIfNotEqual(parser->Eval(), vcl_log(10.0), "UserDefinedCst");
}

void otbParserXTest_UserDefinedFun(void)
{
  ParserType::Pointer parser = ParserType::New();
  parser->SetExpr("ndvi(100, 10)");
  otbParserXTest_ThrowIfNotEqual(parser->Eval(), (10.0-100.0)/(10.0+100.0), "UserDefinedFun");
}

void otbParserXTest_UserDefinedVars(void)
{
  ParserType::ValueType var1 = 10.0;
  ParserType::ValueType var2 = 2.0;
  ParserType::ValueType var3 = 7.0;
  ParserType::ValueType var4 = 100.0;
  ParserType::ValueType var5 = 200.0;

  double dvar1 = 10.0;
  double dvar2 = 2.0;
  double dvar3 = 7.0;
  double dvar4 = 100.0;
  double dvar5 = 200.0;

  ParserType::Pointer parser = ParserType::New();
  parser->DefineVar("var1", &var1);
  parser->DefineVar("var2", &var2);
  parser->DefineVar("var3", &var3);
  parser->DefineVar("var4", &var4);
  parser->DefineVar("var5", &var5);
  parser->SetExpr("(var1+var2-var3)*var4/var5");
  otbParserXTest_ThrowIfNotEqual(parser->Eval(), (dvar1+dvar2-dvar3)*dvar4/dvar5, "UserDefinedVars");
}

void otbParserXTest_Mixed(void)
{
  ParserType::Pointer parser = ParserType::New();
  parser->SetExpr("(7+10)/2+cos(pi/4)*10-10*ln10+ndvi(100, 10)");
  otbParserXTest_ThrowIfNotEqual(parser->Eval(),
                                (7.0+10.0)/2.0+vcl_cos(otb::CONST_PI/4)*10.0-10.0*vcl_log(10.0)+(10.0-100.0)/(10.0+100.0),
                                "Mixed");
}

void otbParserXTest_LogicalOperator(void)
{
  ParserType::Pointer parser = ParserType::New();
  parser->SetExpr("true and false");
  otbParserXTest_ThrowIfNotEqual(static_cast<bool>(parser->Eval()), false, "LogicalOperator and");
  parser->SetExpr("true or false");
  otbParserXTest_ThrowIfNotEqual(static_cast<bool>(parser->Eval()), true, "LogicalOperator or");
}

int otbParserXTest(int itkNotUsed(argc), char * itkNotUsed(argv) [])
{
  otbParserXTest_Numerical();
  otbParserXTest_BuildInFun();
  otbParserXTest_UserDefinedCst();
  otbParserXTest_UserDefinedFun();
  otbParserXTest_UserDefinedVars();
  otbParserXTest_Mixed();
  otbParserXTest_LogicalOperator();
  return EXIT_SUCCESS;
}