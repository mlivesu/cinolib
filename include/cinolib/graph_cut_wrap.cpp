/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2016: Marco Livesu                                              *
*                                                                               *
*  The MIT License                                                              *
*                                                                               *
*  Permission is hereby granted, free of charge, to any person obtaining a      *
*  copy of this software and associated documentation files (the "Software"),   *
*  to deal in the Software without restriction, including without limitation    *
*  the rights to use, copy, modify, merge, publish, distribute, sublicense,     *
*  and/or sell copies of the Software, and to permit persons to whom the        *
*  Software is furnished to do so, subject to the following conditions:         *
*                                                                               *
*  The above copyright notice and this permission notice shall be included in   *
*  all copies or substantial portions of the Software.                          *
*                                                                               *
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR   *
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,     *
*  FITNESS FOR A PARTICULAR PURPOSE AND NON INFRINGEMENT. IN NO EVENT SHALL THE *
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER       *
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING      *
*  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS *
*  IN THE SOFTWARE.                                                             *
*                                                                               *
*  Author(s):                                                                   *
*                                                                               *
*     Marco Livesu (marco.livesu@gmail.com)                                     *
*     http://pers.ge.imati.cnr.it/livesu/                                       *
*                                                                               *
*     Italian National Research Council (CNR)                                   *
*     Institute for Applied Mathematics and Information Technologies (IMATI)    *
*     Via de Marini, 6                                                          *
*     16149 Genoa,                                                              *
*     Italy                                                                     *
*********************************************************************************/
#include <cinolib/graph_cut_wrap.h>

namespace cinolib
{

#ifdef CINOLIB_USES_GRAPH_CUT

template<class M, class V, class E, class P>
CINO_INLINE
std::vector<int> graph_cut_wrap(const AbstractMesh<M,V,E,P>             & m,
                                const int                                 n_labels,
                                const std::vector<double>               & data_term,
                                      GCoptimization::SmoothCostFnExtra   smooth_term,
                                      void                              * smooth_data)
{
    std::vector<int> labels(m.num_polys(), -1);

    try
    {
        GCoptimizationGeneralGraph gc = GCoptimizationGeneralGraph(m.num_polys(), n_labels);

        // set graph connectivity
        for(uint pid=0; pid<m.num_polys(); ++pid)
        for(uint nbr : m.adj_p2p(pid))
        {
            if (pid > nbr) gc.setNeighbors(pid, nbr);
        }

        // set data terms
        for(uint pid=0; pid<m.num_polys(); ++pid)
        for(int label=0; label<n_labels; ++label)
        {
            gc.setDataCost(pid, label, data_term.at(pid*n_labels + label));
        }

        gc.setSmoothCost(smooth_term, smooth_data);
        double energy = gc.swap(-1);

        std::cout << "graph cut energy: " << energy << std::endl;

        for(uint pid=0; pid<m.num_polys(); ++pid)
        {
            labels.at(pid) = gc.whatLabel(pid);
        }
    }
    catch (GCException e)
    {
        std::cout << "\n\n!!!GRAPH-CUT EXCEPTION!!!\nCheck logfile\n\n" << std::endl;
        e.Report();
    }

    return labels;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
std::vector<int> graph_cut_wrap(const AbstractMesh<M,V,E,P>             & m,
                                const int                                 n_labels,
                                const std::vector<std::vector<uint>>    & feasible_region,
                                const std::vector<std::vector<double>>  & data_term,
                                      GCoptimization::SmoothCostFnExtra   smooth_term,
                                      void                              * smooth_data)
{
    std::vector<int> labels(m.num_polys(), -1);

    try
    {
        GCoptimizationGeneralGraph gc = GCoptimizationGeneralGraph(m.num_polys(), n_labels);

        // set graph connectivity
        for(uint pid=0; pid<m.num_polys(); ++pid)
        for(uint nbr : m.adj_p2p(pid))
        {
            if (pid > nbr) gc.setNeighbors(pid, nbr);
        }

        // set data terms
        assert(feasible_region.size() == (size_t)n_labels);
        for(int label=0; label<n_labels; ++label)
        {
            uint count = 0;
            GCoptimization::SparseDataCost data[feasible_region.at(label).size()];

            for(uint i=0; i<feasible_region.at(label).size(); ++i)
            {
                GCoptimization::SparseDataCost record;
                record.site = feasible_region.at(label).at(i);
                record.cost = data_term.at(label).at(i);
                assert(record.cost >= 0);
                data[count++] = record;
            }

            gc.setDataCost(label, data, count);
        }

        gc.setSmoothCost(smooth_term, smooth_data);
        double energy = gc.swap(-1);

        std::cout << "graph cut energy: " << energy << std::endl;

        for(uint pid=0; pid<m.num_polys(); ++pid)
        {
            labels.at(pid) = gc.whatLabel(pid);
        }
    }
    catch (GCException e)
    {
        std::cout << "\n\n!!!GRAPH-CUT EXCEPTION!!!\nCheck logfile\n\n" << std::endl;
        e.Report();
    }

    return labels;
}


#else
template<class M, class V, class E, class P>
CINO_INLINE
std::vector<int> graph_cut_wrap(const AbstractMesh<M,V,E,P>             &,
                                const int                                ,
                                const std::vector<double>               &,
                                      GCoptimization::SmoothCostFnExtra  ,
                                      void                              *)
{
    std::cerr << "ERROR : Graph Cut disabled. Recompile defining symbol CINOLIB_USES_GRAPH_CUT" << std::endl;
    exit(-1);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P>
CINO_INLINE
std::vector<int> graph_cut_wrap(const AbstractMesh<M,V,E,P>             &,
                                const int                                ,
                                const std::vector<std::vector<uint>>    &,
                                const std::vector<std::vector<double>>  &,
                                      GCoptimization::SmoothCostFnExtra  ,
                                      void                              *)
{
    std::cerr << "ERROR : Graph Cut disabled. Recompile defining symbol CINOLIB_USES_GRAPH_CUT" << std::endl;
    exit(-1);
}
#endif

}
