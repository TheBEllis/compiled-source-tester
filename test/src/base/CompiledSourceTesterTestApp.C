//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html
#include "CompiledSourceTesterTestApp.h"
#include "CompiledSourceTesterApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "MooseSyntax.h"

InputParameters
CompiledSourceTesterTestApp::validParams()
{
  InputParameters params = CompiledSourceTesterApp::validParams();
  params.set<bool>("use_legacy_material_output") = false;
  params.set<bool>("use_legacy_initial_residual_evaluation_behavior") = false;
  return params;
}

CompiledSourceTesterTestApp::CompiledSourceTesterTestApp(InputParameters parameters) : MooseApp(parameters)
{
  CompiledSourceTesterTestApp::registerAll(
      _factory, _action_factory, _syntax, getParam<bool>("allow_test_objects"));
}

CompiledSourceTesterTestApp::~CompiledSourceTesterTestApp() {}

void
CompiledSourceTesterTestApp::registerAll(Factory & f, ActionFactory & af, Syntax & s, bool use_test_objs)
{
  CompiledSourceTesterApp::registerAll(f, af, s);
  if (use_test_objs)
  {
    Registry::registerObjectsTo(f, {"CompiledSourceTesterTestApp"});
    Registry::registerActionsTo(af, {"CompiledSourceTesterTestApp"});
  }
}

void
CompiledSourceTesterTestApp::registerApps()
{
  registerApp(CompiledSourceTesterApp);
  registerApp(CompiledSourceTesterTestApp);
}

/***************************************************************************************************
 *********************** Dynamic Library Entry Points - DO NOT MODIFY ******************************
 **************************************************************************************************/
// External entry point for dynamic application loading
extern "C" void
CompiledSourceTesterTestApp__registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  CompiledSourceTesterTestApp::registerAll(f, af, s);
}
extern "C" void
CompiledSourceTesterTestApp__registerApps()
{
  CompiledSourceTesterTestApp::registerApps();
}
