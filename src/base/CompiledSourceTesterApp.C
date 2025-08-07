#include "CompiledSourceTesterApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"
#include "MooseSyntax.h"

InputParameters
CompiledSourceTesterApp::validParams()
{
  InputParameters params = MooseApp::validParams();
  params.set<bool>("use_legacy_material_output") = false;
  params.set<bool>("use_legacy_initial_residual_evaluation_behavior") = false;
  return params;
}

CompiledSourceTesterApp::CompiledSourceTesterApp(InputParameters parameters) : MooseApp(parameters)
{
  CompiledSourceTesterApp::registerAll(_factory, _action_factory, _syntax);
}

CompiledSourceTesterApp::~CompiledSourceTesterApp() {}

void
CompiledSourceTesterApp::registerAll(Factory & f, ActionFactory & af, Syntax & syntax)
{
  ModulesApp::registerAllObjects<CompiledSourceTesterApp>(f, af, syntax);
  Registry::registerObjectsTo(f, {"CompiledSourceTesterApp"});
  Registry::registerActionsTo(af, {"CompiledSourceTesterApp"});

  /* register custom execute flags, action syntax, etc. here */
}

void
CompiledSourceTesterApp::registerApps()
{
  registerApp(CompiledSourceTesterApp);
}

/***************************************************************************************************
 *********************** Dynamic Library Entry Points - DO NOT MODIFY ******************************
 **************************************************************************************************/
extern "C" void
CompiledSourceTesterApp__registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  CompiledSourceTesterApp::registerAll(f, af, s);
}
extern "C" void
CompiledSourceTesterApp__registerApps()
{
  CompiledSourceTesterApp::registerApps();
}
