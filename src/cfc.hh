
#ifndef CFC_HH
#define CFC_HH

#ifdef USE_PYTHON
#include "Python.h"

#include <clipper/core/coords.h>
#include "coot-utils/residue-and-atom-specs.hh"

// return a Python object that contains (with indexing)
//   water positions around ligand
//   chemical features of ligands
//
// environment_residues_py is a list of residue specs
// solvated_ligand_info_py is a list of
//   list mol_no ligand_spec
// which, with radius will be used to find the waters
// 
PyObject *chemical_feature_clusters_py(PyObject *environment_residues_py,
				       PyObject *solvated_ligand_info_py,
				       double radius_1, double radius_2);
// scipy has done some clustering
// we get the cluster info here
// 
void chemical_feature_clusters_accept_info_py(PyObject *env_residue_py,
					      PyObject *mol_ligand_specs_py,
					      PyObject *cluster_info_py);

namespace cfc {

   // the water cluster spec
   //
   class water_cluster_info_from_python {
   public:
      water_cluster_info_from_python(const clipper::Coord_orth &pos_in,
				     double w_in,
				     double r_in) {
	 pos = pos_in;
	 weight = w_in;
	 radius = r_in;
      }
      water_cluster_info_from_python() {weight = 0;}
      clipper::Coord_orth pos;
      double weight;
      double radius;
   };

   // the container for the water residues, annotated by which cluster the water is in
   // 
   class clustered_water_info_from_python {
   public:
      clustered_water_info_from_python(int imol_in, coot::residue_spec_t &spec_in, int cluster_number_in) {
	 imol = imol_in;
	 residue_spec = spec_in;
	 cluster_number = cluster_number_in;
      }
      clustered_water_info_from_python() { imol = -1; cluster_number = -1;}
      int imol;
      int cluster_number;
      coot::residue_spec_t residue_spec;
   };

   class extracted_cluster_info_from_python {
   public:
      std::vector<water_cluster_info_from_python>   wc;
      std::vector<clustered_water_info_from_python> cw;
      extracted_cluster_info_from_python(PyObject *cluster_info_py);
      unsigned int n_structures() const;
      std::vector<int> structures_vec() const;
      unsigned int cluster_idx_max() const;

//       // sort by the number of molecules (structures) are present in this cluster
//       static bool cluster_vector_sorter(const std::vector<int> &v1, const std::vector<int> &v2) {
// 	 return (v2.size() < v1.size());
//       }

      // sort by the number of molecules (structures) are present in this cluster
      static bool cluster_vector_sorter(const std::pair<std::vector<int>, water_cluster_info_from_python> &v1,
					const std::pair<std::vector<int>, water_cluster_info_from_python> &v2) {
	 return (v2.first.size() < v1.first.size());
      }
      // return the generic display object index
      int show_water_balls() const;
   };

}
   



#endif // USE_PYTHON

#endif // CFC_HH
