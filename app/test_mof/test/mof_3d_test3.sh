#!/bin/bash
: <<'END'
This file is part of the Ristra tangram project.
Please see the license file at the root of this repository, or at:
https://github.com/laristra/tangram/blob/master/LICENSE
END


# Exit on error
set -e
# Echo each command
set -x

# 3D MOF interface reconstruction on a voronoi mesh
# with two planar material interfaces forming a T-junction.
# Mesh cells ARE decomposed into tetrahedrons.
# Uses SimpleMesh.
${RUN_COMMAND} ${TESTAPPDIR}/test_mof_3d 1 voronoi95.exo

# Compare the values for the field
${CMPAPPDIR}/apptest_cmp cell_sym_diff_gold3_3d.txt cell_sym_diff_voronoi95_decomposed.txt 1e-09
