#include "CompiledSourceTestingProblem.h"
#include "PhotonSharingData.h"

registerMooseObject("CompiledSourceTesterApp", CompiledSourceTestingProblem);

InputParameters
CompiledSourceTestingProblem::validParams()
{
  InputParameters params = ExternalProblem::validParams();
  params.addRequiredParam<int>("strength_type",
                               "Parameter to determine whether to use uniform source term that's "
                               "identical for all elements, or whether to have a steadily "
                               "increasing strength source term in the x direction.");
  return params;
}

CompiledSourceTestingProblem::CompiledSourceTestingProblem(const InputParameters & params)
  : ExternalProblem(params)
{

  std::ofstream centroid_out("centroids.csv");
  int strength_type = getParam<int>("strength_type");
  for (libMesh::Elem * element_ptr : _mesh.getMesh().active_local_element_ptr_range())
  {
    std::vector<double> energy_spectra(24, 1);

    if (strength_type == 1)
    {
      // If source_type = 1, then make our source strentch increase in the z direction, just to give
      // a good visual and test different element strengths
      double x_val = element_ptr->true_centroid()(0) + 50;
      centroid_out << x_val << ",";
      energy_spectra.assign(24, x_val);
    }

    _photon_fluxes.insert(std::pair<int, std::vector<double>>(element_ptr->id(), energy_spectra));
    insertElementStrength(element_ptr, energy_spectra);
    updateLocalDomainStrength(element_ptr);
  }
  _photon_bins = {1e-11, 0.01, 0.02, 0.05, 0.1, 0.2, 0.3, 0.4, 0.6, 0.8, 1,  1.22, 1.44,
                  1.66,  2,    2.5,  3,    4,   5,   6.5, 8,   10,  12,  14, 20};
  for (auto & bin : _photon_bins)
  {
    if (bin < 0.001)
    {
      continue;
    }
    bin *= 1e6;
  }
  std::cout << _photon_bins[0] << " " << _photon_bins[1] << std::endl;
  getTotalDomainStrength();
}

void
CompiledSourceTestingProblem::externalSolve()
{
}

void
CompiledSourceTestingProblem::syncSolutions(ExternalProblem::Direction direction)
{
  if (direction == ExternalProblem::Direction::FROM_EXTERNAL_APP)
  {

#ifdef LIBMESH_HAVE_BOOST
    {
      // vector of all local domain strengths
      std::vector<double> local_domain_strengths;
      comm().allgather(_local_domain_strength, local_domain_strengths);

      // calculate TotalDomainStrength
      getTotalDomainStrength();

      //
      unsigned long shared_memory_size = calculateMemorySize();
      // Use namespace alias for ease
      namespace bi = boost::interprocess;

      // Give the shared memory region a name based on current MPI rank to
      // prevent clashes
      std::string data_name = "SHARING_DATA_" + std::to_string(comm().rank());

      // Remove any shared memory region with a similar name just in case
      bi::shared_memory_object::remove(data_name.c_str());

      // Calculate space needed for shared mem region

      // Create shared memory region
      bi::managed_shared_memory segment(bi::create_only, data_name.c_str(), shared_memory_size);

      // Create a shared instantiation of the photon sharing class
      PhotonSharingData * photon_sharing_instance = segment.construct<PhotonSharingData>(
          "PhotonSharingData photon_sharing_instance")(segment,
                                                       _photon_fluxes,
                                                       _element_strengths,
                                                       24,
                                                       (int)_mesh.getMesh().n_active_local_elem(),
                                                       _total_domain_strength,
                                                       _local_domain_strength,
                                                       local_domain_strengths,
                                                       _photon_bins);
    }
#endif
  }
}

double
CompiledSourceTestingProblem::calculateElementStrength(const libMesh::Elem * element,
                                                       const std::vector<double> element_flux)
{
  double element_strength = 0;

  for (double flux_bin : element_flux)
  {
    // element_strength += flux_bin * element->volume();

    element_strength += flux_bin;
  }

  return element_strength;
}

void
CompiledSourceTestingProblem::insertElementStrength(const libMesh::Elem * element,
                                                    const std::vector<double> element_flux)
{
  double elem_strength = calculateElementStrength(element, element_flux);

  _element_strengths.insert(std::pair<int, double>(element->id(), elem_strength));
}

void
CompiledSourceTestingProblem::updateLocalDomainStrength(const libMesh::Elem * element)
{
  double element_strength;

  // Attempt to retrieve element strength from map, otherwise catch exception
  // and give a useful error
  try
  {
    element_strength = _element_strengths.at(element->id());
  }
  catch (std::out_of_range)
  {
    mooseError("Attempted to access element strength for an element ID that "
               "has not had strength calcalculated");
  }

  _local_domain_strength += element_strength;
}

void
CompiledSourceTestingProblem::getTotalDomainStrength()
{

  _total_domain_strength = _local_domain_strength;
  comm().sum(_total_domain_strength);
}

int
CompiledSourceTestingProblem::calculateMemorySize()
{
  // Get number of active local elements
  int n_local_elem = _mesh.getMesh().n_active_local_elem();

  unsigned long photon_flux_map_size = ((24 * sizeof(double)) + sizeof(int)) * n_local_elem;

  unsigned long element_strengths_map_size = (sizeof(int) + sizeof(double)) * n_local_elem;

  unsigned long memory_size =
      photon_flux_map_size + element_strengths_map_size + (sizeof(int) * 2) + (sizeof(double) * 2);
  // Really naive way of doing this, but currently giving a 20% buffer to
  // account for the memory space required by Boost allocators and such
  return memory_size * 2;
}
