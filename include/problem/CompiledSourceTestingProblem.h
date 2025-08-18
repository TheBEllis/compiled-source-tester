#pragma once
#include "ExternalProblem.h"
#include <string>
#include <unordered_map>

class CompiledSourceTestingProblem : public ExternalProblem
{

public:
  CompiledSourceTestingProblem(const InputParameters & params);

  static InputParameters validParams();

  virtual void externalSolve() override;
  virtual void syncSolutions(ExternalProblem::Direction direction) override;
  virtual bool converged(unsigned int) override { return true; }

private:
  // FISPACT Photon fluxes
  std::unordered_map<int, std::vector<double>> _photon_fluxes;

  std::unordered_map<int, double> _element_strengths;

  std::vector<double> _photon_bins;

  double _local_domain_strength;

  double _total_domain_strength;

  double calculateElementStrength(const libMesh::Elem * element,
                                  const std::vector<double> element_flux);

  void insertElementStrength(const libMesh::Elem * element, const std::vector<double> element_flux);

  void updateLocalDomainStrength(const libMesh::Elem * element);

  void getTotalDomainStrength();

  int calculateMemorySize();
};
