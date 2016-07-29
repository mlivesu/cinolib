#include <cinolib/gradient.h>
#include <cinolib/timer.h>


namespace cinolib
{

typedef Eigen::Triplet<double> Entry;

template <>
CINO_INLINE
Eigen::SparseMatrix<double> gradient<Trimesh>(const Trimesh & m)
{
    timer_start("Compute gradient matrix");

    Eigen::SparseMatrix<double> G(m.num_triangles()*3, m.num_vertices());

    std::vector<Entry> entries;

    for(int tid=0; tid<m.num_triangles(); ++tid)
    {
        int i = m.triangle_vertex_id(tid, 0);
        int j = m.triangle_vertex_id(tid, 1);
        int k = m.triangle_vertex_id(tid, 2);

        vec3d vi = m.vertex(i);
        vec3d vj = m.vertex(j);
        vec3d vk = m.vertex(k);
        vec3d ik = vi - vk;
        vec3d ji = vj - vi;

        vec3d  axis  = m.triangle_normal(tid);
        double angle = M_PI * 0.5;

        rotate(ik, axis, angle);
        rotate(ji, axis, angle);

        double dblA = m.element_mass(tid);

        ik /= dblA;
        ji /= dblA;

        int row = 3 * tid;
        entries.push_back(Entry(row, i, -(ik.x() + ji.x())));
        entries.push_back(Entry(row, j, ik.x()));
        entries.push_back(Entry(row, k, ji.x()));

        row++;
        entries.push_back(Entry(row, i, -(ik.y() + ji.y())));
        entries.push_back(Entry(row, j, ik.y()));
        entries.push_back(Entry(row, k, ji.y()));

        row++;
        entries.push_back(Entry(row, i, -(ik.z() + ji.z())));
        entries.push_back(Entry(row, j, ik.z()));
        entries.push_back(Entry(row, k, ji.z()));
    }

    G.setFromTriplets(entries.begin(), entries.end());

    timer_stop("Compute gradient matrix");

    return G;
}


template<>
CINO_INLINE
Eigen::SparseMatrix<double> gradient<Tetmesh>(const Tetmesh & m)
{
    timer_start("Compute gradient matrix");

    Eigen::SparseMatrix<double> G(m.num_tetrahedra()*3, m.num_vertices());

    std::vector<Entry> entries;

    for(int tid=0; tid<m.num_tetrahedra(); ++tid)
    {
        int A = m.tet_vertex_id(tid, 0);
        int B = m.tet_vertex_id(tid, 1);
        int C = m.tet_vertex_id(tid, 2);
        int D = m.tet_vertex_id(tid, 3);

        double vol3 = std::max(m.tet_volume(tid), 1e-5) * 3.0;

        vec3d n_ABC = m.tet_face_normal(tid, 0) * m.tet_face_area(tid, 0) / vol3;
        vec3d n_ABD = m.tet_face_normal(tid, 1) * m.tet_face_area(tid, 1) / vol3;
        vec3d n_ACD = m.tet_face_normal(tid, 2) * m.tet_face_area(tid, 2) / vol3;
        vec3d n_BCD = m.tet_face_normal(tid, 3) * m.tet_face_area(tid, 3) / vol3;

        int row = 3 * tid;
        entries.push_back(Entry(row, A, n_ABC.x() + n_ABD.x() + n_ACD.x()));
        entries.push_back(Entry(row, B, n_ABC.x() + n_ABD.x() + n_BCD.x()));
        entries.push_back(Entry(row, C, n_ABC.x() + n_ACD.x() + n_BCD.x()));
        entries.push_back(Entry(row, D, n_BCD.x() + n_ABD.x() + n_ACD.x()));

        row++;
        entries.push_back(Entry(row, A, n_ABC.y() + n_ABD.y() + n_ACD.y()));
        entries.push_back(Entry(row, B, n_ABC.y() + n_ABD.y() + n_BCD.y()));
        entries.push_back(Entry(row, C, n_ABC.y() + n_ACD.y() + n_BCD.y()));
        entries.push_back(Entry(row, D, n_BCD.y() + n_ABD.y() + n_ACD.y()));

        row++;
        entries.push_back(Entry(row, A, n_ABC.z() + n_ABD.z() + n_ACD.z()));
        entries.push_back(Entry(row, B, n_ABC.z() + n_ABD.z() + n_BCD.z()));
        entries.push_back(Entry(row, C, n_ABC.z() + n_ACD.z() + n_BCD.z()));
        entries.push_back(Entry(row, D, n_BCD.z() + n_ABD.z() + n_ACD.z()));
    }

    G.setFromTriplets(entries.begin(), entries.end());

    timer_stop("Compute gradient matrix");

    return G;
}

}
