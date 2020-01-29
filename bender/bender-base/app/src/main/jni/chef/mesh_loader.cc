#include <vcg/complex/complex.h>

#include <vcg/complex/algorithms/update/bounding.h>
#include <vcg/complex/algorithms/update/topology.h>
#include <vcg/complex/algorithms/update/normal.h>
#include <vcg/complex/algorithms/update/flag.h>
#include <vcg/complex/algorithms/local_optimization/tri_edge_collapse_quadric_tex.h>

// input output
#include <wrap/io_trimesh/import.h>

class MyVertex;

struct MyUsedTypes : public vcg::UsedTypes<vcg::Use<MyVertex>::AsVertexType>
{
};

class MyVertex : public vcg::Vertex<MyUsedTypes, vcg::vertex::Coord3f, vcg::vertex::Normal3f, vcg::vertex::TexCoord2f>
{
public:
    vcg::math::Quadric<double> &Qd() { return q; }

private:
    vcg::math::Quadric<double> q;
};

class MyMesh : public vcg::tri::TriMesh<std::vector<MyVertex>>
{
};

class MyTriEdgeCollapse : public vcg::tri::TriEdgeCollapseQuadricTex<MyMesh, BasicVertexPair<MyVertex>, MyTriEdgeCollapse, QInfoStandard<MyVertex>>
{
};

void MeshLoader(std::string &file_name)
{
    int CellNum = 100000;
    float CellSize = 0;
    bool DupFace = false;
    int mask = vcg::tri::io::Mask::IOM_ALL;

    MyMesh m;
    vcg::tri::io::ImporterOBJ<MyMesh>::Open(m, file_name.c_str(), mask);
    printf("Input mesh  vn:%i fn:%i\n", m.VN(), m.FN());

    TriEdgeCollapseQuadricTexParameter params;
    params.SetDefaultParams();
    params.QualityThr = .3;
    vcg::LocalOptimization<MyMesh> deci_session(m, &params);

    deci_session.Init<MyTriEdgeCollapse>();
    printf("Initial Heap Size %i\n", int(deci_session.h.size()));


    while (deci_session.DoOptimization() && m.VertexNumber() > 65536)
        printf("Current Mesh size %7i heap sz %9i err %9g \r", m.VertexNumber(), int(deci_session.h.size()), deci_session.currMetric);

    printf("mesh  %d %d Error %g \n", m.vn, m.fn, deci_session.currMetric);

}
