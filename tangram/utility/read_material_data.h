/*
 This file is part of the Ristra tangram project.
 Please see the license file at the root of this repository, or at:
 https://github.com/laristra/tangram/blob/master/LICENSE
*/

#ifndef READ_MATERIAL_DATA_H_
#define READ_MATERIAL_DATA_H_

#include <stdlib.h>
#include "tangram/support/tangram.h"

/*!
 @brief Reads material data for a mesh from a provided binary file.
 @tparam Mesh_Wrapper A lightweight wrapper to a specific input mesh
                      implementation that provides required functionality.
 @tparam D            Dimensionality of problem

 @param[in] mesh Mesh wrapper.
 @param[in] mesh_data_fname Name of the file with material data.
 @param[out] cell_num_mats Number of material in each mesh cell, vector of length cell_num
 @param[out] cell_mat_ids Indices of materials in each mesh cell, a flat vector, requires
                          computations of offsets
 @param[out] cell_mat_volfracs Volume fractions of materials in each mesh cell, a flat
                               vector, requires computations of offsets
 @param[out] cell_mat_centroids Centroids of materials in each mesh cell, a flat vector,
                                requires computations of offsets
*/
template <class Mesh_Wrapper, int D>
void read_material_data(const Mesh_Wrapper& mesh,
                        const std::string& mesh_data_fname,
                        std::vector<int>& cell_num_mats,
                        std::vector<int>& cell_mat_ids,
                        std::vector<double>& cell_mat_volfracs,
                        std::vector< Tangram::Point<D> >& cell_mat_centroids) {
  std::ifstream os(mesh_data_fname.c_str(), std::ifstream::binary);
  if (!os.good()) {
    std::ostringstream os;
    os << std::endl << "Cannot open " << mesh_data_fname <<
      " for binary input" << std::endl;
    throw std::runtime_error(os.str());
  }

  int data_dim;
  os.read(reinterpret_cast<char *>(&data_dim), sizeof(int));
  assert(data_dim == D);
  int ncells;
  os.read(reinterpret_cast<char *>(&ncells), sizeof(int));
  assert(ncells = mesh.num_owned_cells());
  cell_num_mats.resize(ncells);
 
  int nmatpoly = 0;
  cell_mat_ids.clear();
  std::vector<int> offset(ncells+1, 0);
  for (int icell = 0; icell < ncells; icell++) {
    os.read(reinterpret_cast<char *>(&cell_num_mats[icell]), sizeof(int));

    nmatpoly += cell_num_mats[icell];
    offset[icell + 1] = offset[icell] + cell_num_mats[icell];

    for (int im = 0; im < cell_num_mats[icell]; im++) {
      int imat;
      os.read(reinterpret_cast<char *>(&imat), sizeof(int));
      cell_mat_ids.push_back(imat);      
    }
  }

  cell_mat_volfracs.resize(nmatpoly);
  for (int icell = 0; icell < ncells; icell++) {
    if (cell_num_mats[icell] == 1) {
      cell_mat_volfracs[offset[icell]] = 1.0;
      continue;
    }
    for (int im = 0; im < cell_num_mats[icell]; im++)
      os.read(reinterpret_cast<char *>(&cell_mat_volfracs[offset[icell] + im]), sizeof(double));
  }

  cell_mat_centroids.resize(nmatpoly);
  for (int icell = 0; icell < ncells; icell++) {
    if (cell_num_mats[icell] == 1) {
      mesh.cell_centroid(icell, &cell_mat_centroids[offset[icell]]);
      continue;
    }
    for (int im = 0; im < cell_num_mats[icell]; im++)
      for (int idim = 0; idim < D; idim++)
        os.read(reinterpret_cast<char *>(&cell_mat_centroids[offset[icell] + im][idim]), 
          sizeof(double));
  }

  os.close();
}

#endif
